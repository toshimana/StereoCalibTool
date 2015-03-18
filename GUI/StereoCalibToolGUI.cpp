#include "StereoCalibToolGUI.h"
#include "ui_StereoCalibToolGUI.h"

#include <iostream>

#include <Qtimer>
#include <QDebug>
#include "StereoCaptureActor.h"

namespace MainActorMessage
{
	struct ExecFunc
	{
		ExecFunc( std::function<void( void )> _func ) : func( _func ){}
		std::function<void( void )> func;
	};
	typedef boost::variant<ExecFunc> Message;
};

struct MainActor : public ActorBase < MainActorMessage::Message >
{
private:
	void processMessage( std::shared_ptr<MainActorMessage::Message> msg )
	{
		MessageVisitor mv( this );
		boost::apply_visitor( mv, *msg );
	}

	class MessageVisitor : public boost::static_visitor < void >
	{
	public:
		MessageVisitor( MainActor* const obj ) : base( obj ){}

		void operator()( const MainActorMessage::ExecFunc& msg )
		{
			msg.func();
		}

	private:
		MainActor* const base;
	};
};

struct StereoCalibToolGUI::Impl
{
	StereoCalibToolGUI* const base;

	Ui::ReconstClass ui;
	QTimer* timer;
	MainActor mActor;
	StereoCaptureActor stereoCapture;

	Impl( StereoCalibToolGUI* const obj)
		: base( obj )
	{
		ui.setupUi( obj );

		// GUI Actor‚ÌÝ’è

		// 
		stereoCapture.connectCaptureImage( [this]( const cv::Mat& leftImage, const cv::Mat& rightImage ){
			mActor.entry( MainActorMessage::ExecFunc(
				std::bind( &StereoCalibToolGUI::Impl::captureImage, this, leftImage, rightImage )
				) );
		} );

		stereoCapture.entry( StereoCaptureMessage::Initialize( 2, 1 ) );

		timer = new QTimer();
		connect( timer, SIGNAL( timeout() ), base, SLOT( getMessage() ) );
		timer->start( 10 );
	}

	~Impl(){
		std::cout << __FUNCTION__ << std::endl;
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
