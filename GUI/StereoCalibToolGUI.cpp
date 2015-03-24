#include "StereoCalibToolGUI.h"
#include "ui_StereoCalibToolGUI.h"

#include <iostream>

#include <Qtimer>
#include <QDebug>
#include "StereoCaptureActor.h"
#include "FindStereoFeaturesActor.h"
#include <MainActor.hpp>

#include <opencv2/calib3d/calib3d.hpp>
#include "WaitSync.hpp"
#include "CornerInfo.h"

struct StereoCalibToolGUI::Impl
{
	StereoCalibToolGUI* const base;

	typedef std::pair<cv::Mat, cv::Mat> StereoMat;

	Ui::ReconstClass ui;
	MainActor mActor;
	StereoCaptureActor stereoCapture;
	FindStereoFeaturesActor findStereoFeaturesActor;
	WaitSync<bool, StereoMat> waitFSFA;
	QTimer timer;

	Impl( StereoCalibToolGUI* const obj)
		: base( obj )
	{
		ui.setupUi( obj );


		waitFSFA.connectSync( [this]( bool calcedFlag, StereoMat stereoMat ){
			// findStereoFeaturesActorに特徴探索を実施するMSGを送る
			const cv::Mat  leftImage = stereoMat.first;
			const cv::Mat rightImage = stereoMat.second;
			findStereoFeaturesActor.entry( FindStereoFeaturesMessage::Find( leftImage, rightImage,
				// 結果が完了した後の処理を登録する
				[this, leftImage, rightImage]( CornerInfo leftInfo, CornerInfo rightInfo ){
				waitFSFA.setFirst( true );
				// メインスレッドに描画命令を送る
				mActor.entry( MainActorMessage::ExecFunc( [this, leftImage, leftInfo, rightImage, rightInfo]( void ){
					if ( leftInfo.isEnable() && rightInfo.isEnable() ) {
						findFeatures( leftImage, leftInfo, ui.LeftFeatureImageWidget );
						findFeatures( rightImage, rightInfo, ui.RightFeatureImageWidget );
					}
				} ) );
			} ) );
		} );
		waitFSFA.setFirst( true ); // 初回は既にFSFAの処理が完了しているものとする
		
		stereoCapture.connectCaptureImage( [this]( const cv::Mat leftImage, const cv::Mat rightImage ){
			this->mActor.entry( MainActorMessage::ExecFunc( [this, leftImage, rightImage]( void ){
				this->captureImage( leftImage, rightImage );
			}
			) );
			waitFSFA.setSecond( std::make_pair( leftImage, rightImage ) );
		} );

		stereoCapture.entry( StereoCaptureMessage::Initialize( 2, 1 ) );

		connect( &timer, SIGNAL( timeout() ), base, SLOT( getMessage() ) );
		timer.start( 10 );
	}

	~Impl(){
		stereoCapture.finalize();
		findStereoFeaturesActor.finalize();
	}

	void captureImage( const cv::Mat& leftImage, const cv::Mat& rightImage )
	{
		ui.LeftImageWidget->setImage( leftImage );
		ui.RightImageWidget->setImage( rightImage );
	}

	void findFeatures( const cv::Mat& image, CornerInfo info, ImageFitWidget* widget )
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
