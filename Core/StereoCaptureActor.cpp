#include "StereoCaptureActor.h"

#include <boost/signals2/signal.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>

#include <opencv2/videoio/videoio.hpp>

struct StereoCaptureActor::Impl
{

	struct Machine_ : boost::msm::front::state_machine_def < Machine_ >
	{
		Machine_(StereoCaptureActor* obj):base(obj){}

		// State
		struct StopState : boost::msm::front::state < > {};

		struct CaptureState : boost::msm::front::state < > {
			template <class Event, class Fsm>
			void on_entry( Event const& evt, Fsm& fsm )
			{
				fsm.base->entry( StereoCaptureMessage::Capture() );
			}
		};

		// function
		bool on_initialize( const StereoCaptureMessage::Initialize& msg )
		{
			bool ret;
			           ret = base->mImpl-> leftVideo.open( msg. leftDevice );
			if ( ret ) ret = base->mImpl->rightVideo.open( msg.rightDevice );
			return ret;
		}

		void on_finalize( const StereoCaptureMessage::Finalize& msg )
		{
			base->mImpl-> leftVideo.release();
			base->mImpl->rightVideo.release();
		}
		
		void on_capture( const StereoCaptureMessage::Capture& msg )
		{
			cv::Mat leftImage, rightImage;
			base->mImpl->leftVideo  >> leftImage;
			base->mImpl->rightVideo >> rightImage;

			base->mImpl->captureImage( leftImage, rightImage );

			base->entry( StereoCaptureMessage::Capture() );
		}

		// Transition
		struct transition_table : boost::mpl::vector<
			g_row < StopState,    StereoCaptureMessage::Initialize, CaptureState, &on_initialize >,
			a_row < CaptureState, StereoCaptureMessage::Capture,    CaptureState, &on_capture >,
			a_row < CaptureState, StereoCaptureMessage::Finalize,   StopState,    &on_finalize >
			> {};

		typedef StopState initial_state;

	protected:
		template <class FSM, class Event>
		void no_transition( Event const&, FSM&, int )
		{
			// do nothing.
		}

	private:
		StereoCaptureActor* base;
	};

	typedef boost::msm::back::state_machine<Machine_> Machine;

	class MessageVisitor : public boost::static_visitor < void >
	{
	public:
		MessageVisitor( StereoCaptureActor* const obj ) : base( obj ){}

		template<typename T>
		void operator()( const T& msg ){
			base->mImpl->machine.process_event( msg );
		}
	private:
		StereoCaptureActor* base;
	};

	Machine machine;

	boost::signals2::signal<void( cv::Mat, cv::Mat)> captureImage;

	cv::VideoCapture leftVideo;
	cv::VideoCapture rightVideo;


	Impl( StereoCaptureActor* const obj)
		: machine( obj )
	{
		machine.start();
	}

	~Impl(){
		std::cout << __FUNCTION__ << std::endl;
	}

};

StereoCaptureActor::StereoCaptureActor( void )
	:mImpl(new StereoCaptureActor::Impl( this ))
{}

StereoCaptureActor::~StereoCaptureActor( void )
{
	std::cout << __FUNCTION__ << std::endl;
}

const int*
StereoCaptureActor::getState( void )const
{
	return mImpl->machine.current_state();
}

void
StereoCaptureActor::finalize( void )
{
	entry( StereoCaptureMessage::Finalize() );
	while ( *getState() != 0 ) {
		boost::this_thread::sleep_for( boost::chrono::milliseconds( 1 ) );
	}
}

void 
StereoCaptureActor::connectCaptureImage( std::function<void( cv::Mat, cv::Mat )> func )
{
	mImpl->captureImage.connect( func );
}

void 
StereoCaptureActor::processMessage( std::shared_ptr<StereoCaptureMessage::Message> msg )
{
	Impl::MessageVisitor mv( this );
	boost::apply_visitor( mv, *msg );
}
