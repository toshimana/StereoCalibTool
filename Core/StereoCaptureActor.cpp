#include "StereoCaptureActor.h"

#include <boost/signals2/signal.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>
#include "WaitSync.hpp"

typedef WaitSync<cv::Mat,cv::Mat> WaitStereoCapture;

struct StereoCaptureActor::Impl
{
	// State
	struct StopState    : boost::msm::front::state < > {};
	struct CaptureState : boost::msm::front::state < > {};

	struct Machine_ : boost::msm::front::state_machine_def < Machine_ >
	{
		Machine_(StereoCaptureActor* obj):base(obj){}

		// function
		bool on_initialize( const StereoCaptureMessage::Initialize& msg )
		{
			std::cout << __FUNCTION__ << std::endl;
			return true;
		}
		
		void on_capture( const StereoCaptureMessage::Capture& msg )
		{
			std::cout << __FUNCTION__ << std::endl;
			base->entry( StereoCaptureMessage::Capture() );
		}

		struct transition_table : boost::mpl::vector<
			g_row < StopState,    StereoCaptureMessage::Initialize, CaptureState, &on_initialize >,
			a_row < CaptureState, StereoCaptureMessage::Capture,    CaptureState, &on_capture >
			> {};

		typedef StopState initial_state;

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

	WaitStereoCapture waitStereoCapture;
	boost::signals2::signal<void( cv::Mat, cv::Mat)> captureImage;

	Impl( StereoCaptureActor* const obj)
		: machine( obj )
	{
		machine.start();
	}
};

StereoCaptureActor::StereoCaptureActor( void )
	:mImpl(new StereoCaptureActor::Impl( this ))
{}

StereoCaptureActor::~StereoCaptureActor( void )
{
	
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
