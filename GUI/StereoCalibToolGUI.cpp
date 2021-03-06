#include "StereoCalibToolGUI.h"
#include "ui_StereoCalibToolGUI.h"

#include <iostream>

#include <Qtimer>
#include <QFileDialog>
#include <QDebug>


#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/format.hpp>
#include <opencv2/calib3d/calib3d.hpp>


#include "StereoCaptureActor.h"
#include "FindStereoFeaturesActor.h"
#include "StereoCalibActor.h"
#include <MainActor.hpp>

#include "WaitSync.hpp"
#include "CornerInfo.h"
#include "StereoCameraParam.hpp"

#include "qtsjis.hpp"

struct StereoCalibToolGUI::Impl
{
	StereoCalibToolGUI* const base;

	typedef std::pair<cv::Mat, cv::Mat> StereoMat;

	Ui::ReconstClass ui;
	MainActor mActor;

	StereoCaptureActor stereoCapture;

	FindStereoFeaturesActor findStereoFeaturesActor;
	WaitSync<bool, StereoMat> waitFSFA;

	StereoCalibActor stereoCalibActor;

	QTimer timer;

	SpStereoCameraParam stereoCameraParam;
	void setStereoCameraParam( SpStereoCameraParam p ){ 
		stereoCameraParam = p; 
		if ( p ) ui.SaveParamButton->setEnabled( true );
		else     ui.SaveParamButton->setEnabled( false );
	}

	struct Machine_ : boost::msm::front::state_machine_def < Machine_ >
	{
		Machine_( StereoCalibToolGUI* obj ) :base( obj ){}

		// State
		struct CaptureState : boost::msm::front::state < > {};
		struct StoreState   : boost::msm::front::state < > {
			template <class Event, class Fsm>
			void on_entry( Event const& evt, Fsm& fsm )
			{
				fsm.base->mImpl->ui.StoreButton->setText( "Stop" );
				fsm.base->mImpl->ui.CalibrateButton->setEnabled( false );
			}

			template <class Event, class Fsm>
			void on_exit( Event const& evt, Fsm& fsm )
			{
				fsm.base->mImpl->ui.StoreButton->setText( "Store" );
				fsm.base->mImpl->ui.CalibrateButton->setEnabled( true );
			}
		};
		struct CalibrateState : boost::msm::front::state < > {
			template <class Event, class Fsm>
			void on_entry( Event const& evt, Fsm& fsm )
			{
				fsm.base->mImpl->ui.StoreButton->setEnabled( false );
				fsm.base->mImpl->ui.CalibrateButton->setEnabled( false );
				fsm.base->mImpl->findStereoFeaturesActor.entry( FindStereoFeaturesMessage::Finalize() );
			}

			template <class Event, class Fsm>
			void on_exit( Event const& evt, Fsm& fsm )
			{
				fsm.base->mImpl->findStereoFeaturesActor.entry( FindStereoFeaturesMessage::Initialize() );
				fsm.base->mImpl->waitFSFA.setFirst( true );
				fsm.base->mImpl->ui.StoreButton->setEnabled( true );
				fsm.base->mImpl->ui.CalibrateButton->setEnabled( true );
			}
		};

		// Event
		struct StoreEvent{};
		struct CalibrateEvent{};
		struct CalibFinishEvent{};

		// Function
		bool on_calibrate( const CalibrateEvent& msg )
		{
			ImageListWidget::SpPoints3f objectPoints;
			ImageListWidget::SpPoints2f leftImagePoints, rightImagePoints;
			cv::Size imageSize;
			base->mImpl->ui.FeaturesListWidget->getFeatures( objectPoints, leftImagePoints, rightImagePoints, imageSize );

			base->mImpl->stereoCalibActor.entry( StereoCalibMessage::Calibrate( objectPoints, leftImagePoints, rightImagePoints, imageSize, 
				[this](SpStereoCameraParam p){
				this->base->mImpl->mActor.entry( MainActorMessage::ExecFunc( [this,p]() {
					std::string msg = (boost::format( "Reproject Error : %d" ) % p->err).str();
					this->base->statusBar()->showMessage( msg.c_str() );
					this->base->mImpl->setStereoCameraParam( p );
					this->base->mImpl->machine.process_event( Machine::CalibFinishEvent() );
				} ) );
			} ) );

			return true;
		}

		// Transition
		struct transition_table : boost::mpl::vector <
			_row < CaptureState, StoreEvent, StoreState   >,
			_row < StoreState,   StoreEvent, CaptureState >,
			g_row< CaptureState, CalibrateEvent, CalibrateState, &on_calibrate >,
			_row < CalibrateState, CalibFinishEvent, CaptureState >
		> {};

		enum STATE_ID {
			CAPTURE_STATE_ID = 0,
			STORE_STATE_ID,
		};

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

		waitFSFA.connectSync( [this]( bool calcedFlag, StereoMat stereoMat ){
			// findStereoFeaturesActorに特徴探索を実施するMSGを送る
			const cv::Mat  leftImage = stereoMat.first;
			const cv::Mat rightImage = stereoMat.second;
			findStereoFeaturesActor.entry( FindStereoFeaturesMessage::Find( leftImage, rightImage,
				// 結果が完了した後の処理を登録する
				[this, leftImage, rightImage]( CornerInfo leftInfo, CornerInfo rightInfo ){
				waitFSFA.setFirst( true );
				// メインスレッドに描画命令を送る
				mActor.entry( MainActorMessage::ExecFunc( std::bind(&StereoCalibToolGUI::Impl::findStereoFeatures, this, leftImage, leftInfo, rightImage, rightInfo ) ) );
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
		findStereoFeaturesActor.entry( FindStereoFeaturesMessage::Initialize() );

		ui.StoreButton->connectPressed( [this](){ 
			machine.process_event( Machine::StoreEvent() );
		} );

		ui.CalibrateButton->connectPressed( [this](){
			machine.process_event( Machine::CalibrateEvent() );
		} );

		ui.SaveParamButton->connectPressed( [this](){
			QString filePath = QFileDialog::getSaveFileName( this->base, QString(), QString(), tr( "XML File (*.xml)" ) );

			this->stereoCameraParam->write( qtsjis::toString( filePath ) );
		} );

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

	void findStereoFeatures( const cv::Mat& leftImage, const CornerInfo& leftInfo, const cv::Mat& rightImage, const CornerInfo& rightInfo )
	{
		if ( leftInfo.isEnable() && rightInfo.isEnable() ) {
			if ( *(machine.current_state()) == static_cast<int>(Machine::STATE_ID::STORE_STATE_ID) ) {
				ui.FeaturesListWidget->addFeatures( leftImage, leftInfo, rightImage, rightInfo );
			}

			findFeatures( leftImage, leftInfo, ui.LeftFeatureImageWidget );
			findFeatures( rightImage, rightInfo, ui.RightFeatureImageWidget );
		}
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
	
	mImpl->machine.start();
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
