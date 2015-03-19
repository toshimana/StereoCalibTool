#include "StereoCalibToolGUI.h"
#include "ui_StereoCalibToolGUI.h"

#include <iostream>

#include <Qtimer>
#include <QDebug>
#include "StereoCaptureActor.h"
#include <MainActor.hpp>

struct StereoCalibToolGUI::Impl
{
	StereoCalibToolGUI* const base;

	Ui::ReconstClass ui;
	MainActor mActor;
	StereoCaptureActor stereoCapture;
	QTimer timer;

	Impl( StereoCalibToolGUI* const obj)
		: base( obj )
	{
		ui.setupUi( obj );

		stereoCapture.connectCaptureImage( [this]( const cv::Mat leftImage, const cv::Mat rightImage ){
			this->mActor.entry( MainActorMessage::ExecFunc( [this, leftImage, rightImage]( void ){
				this->captureImage( leftImage, rightImage );
			}
				) );
		} );

		stereoCapture.entry( StereoCaptureMessage::Initialize( 2, 1 ) );

		connect( &timer, SIGNAL( timeout() ), base, SLOT( getMessage() ) );
		timer.start( 10 );
	}

	~Impl(){
		std::cout << __FUNCTION__ << std::endl;
		stereoCapture.finalize();
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
	std::cout << __FUNCTION__ << std::endl;
}

void 
StereoCalibToolGUI::getMessage( void ) const
{
	mImpl->mActor.receive();
}
