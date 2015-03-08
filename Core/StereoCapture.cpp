#include "StereoCapture.h"

#include <boost/signals2/signal.hpp>
#include "SingleCapture.h"
#include "TaskScheduler.h"
#include "WaitSync.hpp"

typedef WaitSync<cv::Mat,cv::Mat> WaitStereoCapture;

struct StereoCapture::Impl
{
	std::unique_ptr<SingleCapture> leftCapture;
	TaskScheduler leftCaptureThread;

	std::unique_ptr<SingleCapture> rightCapture;
	TaskScheduler rightCaptureThread;

	WaitStereoCapture waitStereoCapture;
	boost::signals2::signal<void( cv::Mat, cv::Mat)> captureImage;

	Impl(void)
		: leftCapture(new SingleCapture())
		, leftCaptureThread(1)
		, rightCapture(new SingleCapture())
		, rightCaptureThread(1)
	{}
};

StereoCapture::StereoCapture( void )
	:mImpl(new StereoCapture::Impl())
{}

StereoCapture::~StereoCapture( void )
{
	
}

int
StereoCapture::initialize( void )
{
	mImpl->waitStereoCapture.connectSync( 
		[this]( const cv::Mat& leftImage, const cv::Mat& rightImage ){ 
		this->mImpl->captureImage( leftImage, rightImage );
	} );

	std::function<void( void )>  leftCapture = [this]( void ){
		this->mImpl-> leftCaptureThread.entry( [this]( void ) { this->mImpl-> leftCapture->capture(); } );
	};
	std::function<void( void )> rightCapture = [this]( void ){
		this->mImpl->rightCaptureThread.entry( [this]( void ) { this->mImpl->rightCapture->capture(); } );
	};

	// ¶ƒJƒƒ‰
	mImpl->leftCapture->connectCaptureImage( [this]( const cv::Mat& image ) {
		this->mImpl->waitStereoCapture.setFirst( image );
	} );
	mImpl->leftCapture->connectCaptureImage( [this,leftCapture]( const cv::Mat& image ){ leftCapture();	} );

	// ‰EƒJƒƒ‰
	mImpl->rightCapture->connectCaptureImage( [this]( const cv::Mat& image ) {
		this->mImpl->waitStereoCapture.setSecond( image );
	} );
	mImpl->rightCapture->connectCaptureImage( [this, rightCapture]( const cv::Mat& image ){ rightCapture(); } );

	/*
	int ret;

	ret = mImpl->leftCapture->initialize( 2 );
	if ( ret < 0 ){
		return ret;
	}

	ret = mImpl->rightCapture->initialize( 1 );
	if ( ret < 0 ){
		return ret;
	}
	*/

	mImpl-> leftCaptureThread.entry( [this]( void ){ this->mImpl-> leftCapture->initialize( 2 ); } );
	mImpl->rightCaptureThread.entry( [this]( void ){ this->mImpl->rightCapture->initialize( 1 ); } );

	leftCapture();
	rightCapture();

	return 0;
}

void 
StereoCapture::connectCaptureImage( std::function<void( cv::Mat, cv::Mat )> func )
{
	mImpl->captureImage.connect( func );
}