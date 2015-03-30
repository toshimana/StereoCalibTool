#pragma once

#include <memory>
#include <functional>
#include <opencv2/core/core.hpp>

#include <Actor.hpp>
#include "CornerInfo.h"
#include "StereoCameraParam.hpp"

namespace StereoCalibMessage
{
	struct Calibrate
	{
		typedef std::vector<std::vector<cv::Point3f> > Points3f;
		typedef std::vector<std::vector<cv::Point2f> > Points2f;
		typedef std::shared_ptr<Points3f> SpPoints3f;
		typedef std::shared_ptr<Points2f> SpPoints2f;

		Calibrate( SpPoints3f op, SpPoints2f lip, SpPoints2f rip, cv::Size is, std::function<void(SpStereoCameraParam)> func )
			: objectPoints( op ), leftImagePoints( lip ), rightImagePoints( rip ), imageSize( is ), deligate( func ){}

		SpPoints3f objectPoints;
		SpPoints2f leftImagePoints;
		SpPoints2f rightImagePoints;
		cv::Size   imageSize;
		std::function<void( SpStereoCameraParam )> deligate;
	};

	typedef boost::variant<Calibrate> Message;
};

class StereoCalibActor : public Actor<StereoCalibMessage::Message>
{
public:
	StereoCalibActor( void );
	~StereoCalibActor( void );

private:
	struct Impl;
	std::unique_ptr<Impl> mImpl;

	void processMessage( std::shared_ptr<StereoCalibMessage::Message> msg );
};
