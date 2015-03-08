#include "SingleCapture.h"

#include <boost/signals2/signal.hpp>
#include <opencv2/highgui/highgui.hpp>

struct SingleCapture::Impl
{
	cv::VideoCapture videoCapture;
	boost::signals2::signal<void( cv::Mat )> captureImage;
	Impl( void )
	{}
};

SingleCapture::SingleCapture( void )
	:mImpl(new SingleCapture::Impl() )
{}

SingleCapture::~SingleCapture( void )
{}

int 
SingleCapture::initialize( int device )
{
	bool ret = mImpl->videoCapture.open( device );
	if ( !ret ) {
		return -1;
	}
	return 0;
}

void
SingleCapture::capture( void )
{
	cv::Mat image;
	mImpl->videoCapture >> image;
	mImpl->captureImage( image );
}

void 
SingleCapture::connectCaptureImage( std::function<void( cv::Mat )> func )
{
	mImpl->captureImage.connect( func );
}
