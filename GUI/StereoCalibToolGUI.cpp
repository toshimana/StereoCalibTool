#include "StereoCalibToolGUI.h"
#include "ui_StereoCalibToolGUI.h"

#include <iostream>

#include <QDebug>
#include "StereoCaptureActor.h"

struct StereoCalibToolGUI::Impl
{
	Ui::ReconstClass ui;

	StereoCaptureActor stereoCapture;

	Impl( StereoCalibToolGUI* const obj)
	{
		ui.setupUi( obj );

		stereoCapture.connectCaptureImage( 
			std::bind( &StereoCalibToolGUI::Impl::captureImage, this, std::placeholders::_1, std::placeholders::_2 ) );
	}

	void captureImage( const cv::Mat& leftImage, const cv::Mat& rightImage )
	{
		ui.LeftImageWidget->setImage( leftImage );
		ui.RightImageWidget->setImage( rightImage );
	}

};

StereoCalibToolGUI::StereoCalibToolGUI(QWidget *parent)
	: QMainWindow(parent)
	, mImpl(new StereoCalibToolGUI::Impl(this))
{
	std::cout << "Run" << std::endl;
}

StereoCalibToolGUI::~StereoCalibToolGUI()
{

}

