#include "StereoCalibToolGUI.h"
#include "ui_StereoCalibToolGUI.h"

#include <iostream>

#include <QTimer>
#include <QDebug>
#include <opencv2/highgui/highgui.hpp>

struct StereoCalibToolGUI::Impl
{
	Ui::ReconstClass ui;
	QTimer* timer;

	cv::VideoCapture leftVideo;
	cv::VideoCapture rightVideo;

	Impl( StereoCalibToolGUI* const obj)
		: leftVideo( 1 )
		, rightVideo( 2 )
	{
		if ( !leftVideo.isOpened() ){
			std::cerr << "Left Camera is not opened." << std::endl;
		}

		if ( !rightVideo.isOpened() ){
			std::cerr << "Right Camera is not opened." << std::endl;
		}

		ui.setupUi( obj );
		timer = new QTimer();

		connect( timer, SIGNAL( timeout() ), obj, SLOT( capture() ) );
		timer->start( 66 );
	}

};

StereoCalibToolGUI::StereoCalibToolGUI(QWidget *parent)
	: QMainWindow(parent)
	, mImpl(new StereoCalibToolGUI::Impl(this))
{
}

StereoCalibToolGUI::~StereoCalibToolGUI()
{

}

void StereoCalibToolGUI::capture()
{
	cv::Mat leftImage, rightImage;

	mImpl->leftVideo  >> leftImage;
	mImpl->rightVideo >> rightImage;

	mImpl->ui.LeftImageWidget->setImage( leftImage );
	mImpl->ui.RightImageWidget->setImage( rightImage );


}