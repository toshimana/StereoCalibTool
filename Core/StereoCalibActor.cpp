#include "StereoCalibActor.h"

#include <opencv2/calib3d/calib3d.hpp>

struct StereoCalibActor::Impl
{
	class MessageVisitor : public boost::static_visitor < void >
	{
	public:
		MessageVisitor( StereoCalibActor* const obj ) : base( obj ){}

		void operator()( const StereoCalibMessage::Calibrate& msg ){
			
			cv::Mat lCameraMatrix, lDistCoeffs;
			cv::Mat rCameraMatrix, rDistCoeffs;

#pragma omp parallel
#pragma omp sections
			{
#pragma omp section
{
	std::vector<cv::Mat> lrvecs, ltvecs;
	cv::calibrateCamera( *(msg.objectPoints), *(msg.leftImagePoints), msg.imageSize, lCameraMatrix, lDistCoeffs, lrvecs, ltvecs );
}
#pragma omp section
{
	std::vector<cv::Mat> rrvecs, rtvecs;
	cv::calibrateCamera( *(msg.objectPoints), *(msg.rightImagePoints), msg.imageSize, rCameraMatrix, rDistCoeffs, rrvecs, rtvecs );
}
			}

			cv::Mat R, T, E, F;
			cv::stereoCalibrate( *(msg.objectPoints), *(msg.leftImagePoints), *(msg.rightImagePoints),
				lCameraMatrix, lDistCoeffs, rCameraMatrix, rDistCoeffs, msg.imageSize, R, T, E, F );
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
