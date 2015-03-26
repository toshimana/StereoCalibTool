#include "FindStereoFeaturesActor.h"

#include <boost/signals2/signal.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <cvtool.hpp>
#include "CornerInfo.h"

typedef std::shared_ptr<CornerInfo> SpCornerInfo;

static void 
findCorner( const cv::Mat& image, SpCornerInfo& info )
{
	cv::Size patternSize( 13, 8 );
	cv::Mat grayImage;
	cvtool::cvtColor2Gray( image, grayImage );
	
	std::shared_ptr<std::vector<cv::Point2f> > corners = std::make_shared<std::vector<cv::Point2f> >();
	bool patternFound =
		cv::findChessboardCorners( grayImage, patternSize, *corners,
		cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE );
	
	if ( patternFound ) {
		cv::cornerSubPix( grayImage, *corners, cv::Size( 5, 5 ), cv::Size( -1, -1 ),
			cv::TermCriteria( cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 20, 1.0 ) );
	}

	info = std::make_shared<CornerInfo>( patternSize, corners, patternFound );
}

struct FindStereoFeaturesActor::Impl
{
	struct Machine_ : boost::msm::front::state_machine_def < Machine_ >
	{
		Machine_(FindStereoFeaturesActor* obj):base(obj){}

		// State
		struct WaitState : boost::msm::front::state < > {};
		struct ProcState : boost::msm::front::state < > {};
		struct HaltState : boost::msm::front::state < > {};

		// Function
		void on_find( const FindStereoFeaturesMessage::Find& msg )
		{
			SpCornerInfo leftInfo, rightInfo;

#pragma omp parallel
#pragma omp sections
			{
#pragma omp section
			{
				findCorner( msg.leftImage, leftInfo );
			}
#pragma omp section
			{
				findCorner( msg.rightImage, rightInfo );
			}
			}

			base->entry( FindStereoFeaturesMessage::Calculated( *leftInfo, *rightInfo, msg.deligate ) );
		}

		void on_calculated( const FindStereoFeaturesMessage::Calculated& msg )
		{
			msg.deligate( msg.leftInfo, msg.rightInfo );
		}

		// Transition
		struct transition_table : boost::mpl::vector<
			a_row < WaitState, FindStereoFeaturesMessage::Find,       ProcState, &on_find >,
			a_row < ProcState, FindStereoFeaturesMessage::Calculated, WaitState, &on_calculated>,
			 _row < WaitState, FindStereoFeaturesMessage::Finalize,   HaltState>,
			 _row < ProcState, FindStereoFeaturesMessage::Finalize,   HaltState>
			> {};

		typedef WaitState initial_state;

	protected:
		template <class FSM, class Event>
		void no_transition( Event const&, FSM&, int )
		{
			// do nothing.
		}


	private:
		FindStereoFeaturesActor* base;
	};

	typedef boost::msm::back::state_machine<Machine_> Machine;

	class MessageVisitor : public boost::static_visitor < void >
	{
	public:
		MessageVisitor( FindStereoFeaturesActor* const obj ) : base( obj ){}

		template<typename T>
		void operator()( const T& msg ){
			base->mImpl->machine.process_event( msg );
		}
	private:
		FindStereoFeaturesActor* base;

	};

	Impl( FindStereoFeaturesActor* const obj)
		: machine( obj )
	{
		machine.start();
	}

	~Impl(){
	}

	Machine machine;
};

FindStereoFeaturesActor::FindStereoFeaturesActor( void )
	:mImpl(new FindStereoFeaturesActor::Impl( this ))
{}

FindStereoFeaturesActor::~FindStereoFeaturesActor( void )
{
}

const int*
FindStereoFeaturesActor::getState( void )const
{
	return mImpl->machine.current_state();
}

void
FindStereoFeaturesActor::finalize( void )
{
	entry( FindStereoFeaturesMessage::Finalize() );
	while ( *getState() != 2 ) {
		boost::this_thread::sleep_for( boost::chrono::milliseconds( 1 ) );
	}
}

void 
FindStereoFeaturesActor::processMessage( std::shared_ptr<FindStereoFeaturesMessage::Message> msg )
{
	Impl::MessageVisitor mv( this );
	boost::apply_visitor( mv, *msg );
}
