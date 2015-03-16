#include "StereoCaptureActor.h"

#include <boost/signals2/signal.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>
#include "WaitSync.hpp"

namespace StereoCaptureStateMachine
{
	// State
	struct StopState    : boost::msm::front::state < > {};
	struct CaptureState : boost::msm::front::state < > {};

	using namespace StereoCaptureMessage;

	struct Machine_ : boost::msm::front::state_machine_def < Machine_ >
	{
		// function
		bool on_initialize( const Initialize& msg ) 
		{
			std::cout << __FUNCTION__ << std::endl;
			return true;
		}

		void on_capture( const Capture& msg )
		{
			std::cout << __FUNCTION__ << std::endl;
			
		}

		struct transition_table : boost::mpl::vector<
			g_row < StopState,    Initialize, CaptureState, &Machine_::on_initialize >,
			a_row < CaptureState, Capture,    CaptureState, &Machine_::on_capture >
			> {};

		typedef StopState initial_state;
	};
}

typedef boost::msm::back::state_machine<StereoCaptureStateMachine::Machine_> Machine;

typedef WaitSync<cv::Mat,cv::Mat> WaitStereoCapture;

struct StereoCaptureActor::Impl
{
	Machine machine;

	WaitStereoCapture waitStereoCapture;
	boost::signals2::signal<void( cv::Mat, cv::Mat)> captureImage;

	Impl(void)
	{
		machine.start();
	}
};

StereoCaptureActor::StereoCaptureActor( void )
	:mImpl(new StereoCaptureActor::Impl())
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

}
