#include "StereoCalibToolGUI.h"
#include "ui_StereoCalibToolGUI.h"

#include <iostream>

#include <Qtimer>
#include <QDebug>
#include "StereoCaptureActor.h"
#include "FindStereoFeaturesActor.h"
#include <MainActor.hpp>

#include <opencv2/calib3d/calib3d.hpp>

struct StereoCalibToolGUI::Impl
{
	StereoCalibToolGUI* const base;

	typedef std::vector<cv::Point2f> Corners;
	typedef std::shared_ptr<Corners> SpCorners;

	Ui::ReconstClass ui;
	MainActor mActor;
	StereoCaptureActor stereoCapture;
	FindStereoFeaturesActor findStereoFeaturesActor;
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
			this->findStereoFeaturesActor.entry( FindStereoFeaturesMessage::Find( leftImage, rightImage, 
				[this, leftImage, rightImage]( CornerInfo leftInfo, CornerInfo rightInfo ){
				this->mActor.entry( MainActorMessage::ExecFunc( [this, leftImage, leftInfo]( void ){
					findFeatures( leftImage, leftInfo, this->ui.LeftFeatureImageWidget );
				} ) );
				this->mActor.entry( MainActorMessage::ExecFunc( [this, rightImage, rightInfo]( void ){
					findFeatures( rightImage, rightInfo, this->ui.RightFeatureImageWidget );
				} ) );
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

	void findFeatures( const cv::Mat& image, CornerInfo info, ImageWidget* widget )
	{
		cv::Mat canvas = image.clone();
		cv::drawChessboardCorners( canvas, info.patternSize, *(info.corners), info.patternWasFound );
		widget->setImage( canvas );
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
