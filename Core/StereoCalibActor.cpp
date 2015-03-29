#include "StereoCalibActor.h"

#include <boost/timer.hpp>
#include <opencv2/calib3d/calib3d.hpp>

struct StereoCalibActor::Impl
{
	struct MeasureTime
	{
		MeasureTime( const std::string& _str )
			: str(_str), timer()
		{
			std::cout << str << " : START" << std::endl;
		}

		~MeasureTime()
		{
			std::cout << str << " : " << timer.elapsed() << std::endl;
		}
		boost::timer timer;
		std::string str;
	};

	class MessageVisitor : public boost::static_visitor < void >
	{
	public:
		MessageVisitor( StereoCalibActor* const obj ) : base( obj ){}

		void operator()( const StereoCalibMessage::Calibrate& msg ){

			MeasureTime cTime( "Calib" );

			cv::Mat lCameraMatrix, lDistCoeffs;
			cv::Mat rCameraMatrix, rDistCoeffs;

#pragma omp parallel
#pragma omp sections
			{
#pragma omp section
{
	MeasureTime lTime( "Left" );
	std::vector<cv::Mat> lrvecs, ltvecs;
	cv::calibrateCamera( *(msg.objectPoints), *(msg.leftImagePoints), msg.imageSize, lCameraMatrix, lDistCoeffs, lrvecs, ltvecs );
}
#pragma omp section
{
	MeasureTime rTime( "Right" );
	std::vector<cv::Mat> rrvecs, rtvecs;
	cv::calibrateCamera( *(msg.objectPoints), *(msg.rightImagePoints), msg.imageSize, rCameraMatrix, rDistCoeffs, rrvecs, rtvecs );
}
			}

			std::cout << lCameraMatrix << std::endl;
			std::cout << lDistCoeffs << std::endl;
			std::cout << rCameraMatrix << std::endl;
			std::cout << rDistCoeffs << std::endl;

			cv::Mat R, T, E, F;
			double err;
			{
				MeasureTime sTime( "Stereo" );
				err = cv::stereoCalibrate( *(msg.objectPoints), *(msg.leftImagePoints), *(msg.rightImagePoints),
					lCameraMatrix, lDistCoeffs, rCameraMatrix, rDistCoeffs, msg.imageSize, R, T, E, F,
					cv::CALIB_FIX_INTRINSIC );
			}

			std::cout << R << std::endl;
			std::cout << T << std::endl;

			msg.deligate( err );
		}
	private:
		StereoCalibActor* base;

	};

	Impl( StereoCalibActor* const obj)
	{
	}

	~Impl(){
	}
};

StereoCalibActor::StereoCalibActor( void )
	:mImpl(new StereoCalibActor::Impl( this ))
{}

StereoCalibActor::~StereoCalibActor( void )
{
}

void 
StereoCalibActor::processMessage( std::shared_ptr<StereoCalibMessage::Message> msg )
{
	Impl::MessageVisitor mv( this );
	boost::apply_visitor( mv, *msg );
}
