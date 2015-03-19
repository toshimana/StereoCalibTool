#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <opencv2/highgui/highgui.hpp>

#include <StereoCaptureActor.h>
#include <MainActor.hpp>

using namespace ::testing;

class StereoCaptureActorTest : public ::testing::Test
{
};

TEST_F( StereoCaptureActorTest, a )
{
	std::shared_ptr<MainActor> mainActor = std::make_shared<MainActor>();
	StereoCaptureActor actor;

	ASSERT_THAT( *(actor.getState()), Eq( 0 ) );

	actor.connectCaptureImage( [mainActor]( cv::Mat leftImage, cv::Mat rightImage ) {
		mainActor->entry( MainActorMessage::ExecFunc( [leftImage, rightImage]( void ){
			cv::imshow( "left", leftImage );
			cv::imshow( "right", rightImage );
			cv::waitKey( 1 );
		} ) );
	} );
	actor.entry( StereoCaptureMessage::Initialize( 2, 1 ) );

	while ( !mainActor->receive() ) {
		boost::this_thread::sleep_for( boost::chrono::milliseconds( 1 ) );
	};
	ASSERT_THAT( *(actor.getState()), Eq( 1 ) );

	actor.finalize();
	ASSERT_THAT( *(actor.getState()), Eq( 0 ) );
}