#include "StereoCalibToolGUI.h"
#include "ui_StereoCalibToolGUI.h"

#include <iostream>

#include <Qtimer>
#include <QDebug>

#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <opencv2/calib3d/calib3d.hpp>


#include "StereoCaptureActor.h"
#include "FindStereoFeaturesActor.h"
#include <MainActor.hpp>

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

	struct Machine_ : boost::msm::front::state_machine_def < Machine_ >
	{
		Machine_( StereoCalibToolGUI* obj ) :base( obj ){}

		// State
		struct CaptureState : boost::msm::front::state < > {};
		struct StoreState   : boost::msm::front::state < > {};

		// Event
		struct StoreEvent{};

		// Function

		// Transition
		struct transition_table : boost::mpl::vector <
			_row < CaptureState, StoreEvent, StoreState >
		> {};

		typedef CaptureState initial_state;

	private:
		StereoCalibToolGUI* base;
	};

	typedef boost::msm::back::state_machine<Machine_> Machine;

	Machine machine;

	Impl( StereoCalibToolGUI* const obj)
		: base( obj )
		, machine( obj )
	{
		ui.setupUi( obj );
		machine.start();

		waitFSFA.connectSync( [this]( bool calcedFlag, StereoMat stereoMat ){
			// findStereoFeaturesActor�ɓ����T�������{����MSG�𑗂�
			const cv::Mat  leftImage = stereoMat.first;
			const cv::Mat rightImage = stereoMat.second;
			findStereoFeaturesActor.entry( FindStereoFeaturesMessage::Find( leftImage, rightImage,
				// ���ʂ�����������̏�����o�^����
				[this, leftImage, rightImage]( CornerInfo leftInfo, CornerInfo rightInfo ){
				waitFSFA.setFirst( true );
				// ���C���X���b�h�ɕ`�施�߂𑗂�
				mActor.entry( MainActorMessage::ExecFunc( [this, leftImage, leftInfo, rightImage, rightInfo]( void ){
					if ( leftInfo.isEnable() && rightInfo.isEnable() ) {
						findFeatures( leftImage, leftInfo, ui.LeftFeatureImageWidget );
						findFeatures( rightImage, rightInfo, ui.RightFeatureImageWidget );
					}
				} ) );
			} ) );
		} );
		waitFSFA.setFirst( true ); // ����͊���FSFA�̏������������Ă�����̂Ƃ���
		
		stereoCapture.connectCaptureImage( [this]( const cv::Mat leftImage, const cv::Mat rightImage ){
			this->mActor.entry( MainActorMessage::ExecFunc( [this, leftImage, rightImage]( void ){
				this->captureImage( leftImage, rightImage );
			}
			) );
			waitFSFA.setSecond( std::make_pair( leftImage, rightImage ) );
		} );

		stereoCapture.entry( StereoCaptureMessage::Initialize( 2, 1 ) );

		ui.StoreButton->connectPressed( [](){ std::cout << "Push!" << std::endl; } );

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
