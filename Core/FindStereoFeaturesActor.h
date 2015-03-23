#pragma once

#include <memory>
#include <functional>
#include <opencv2/core/core.hpp>

#include <Actor.hpp>
#include "CornerInfo.h"

namespace FindStereoFeaturesMessage
{
	struct Find
	{
		Find( const cv::Mat& left, const cv::Mat& right, std::function<void(CornerInfo,CornerInfo)> func ) 
			: leftImage( left ), rightImage( right ){}
		const cv::Mat leftImage;
		const cv::Mat rightImage;
		std::function<void( CornerInfo, CornerInfo )> deligate;
	};

	struct Calculated{};
	struct Finalize{};

	typedef boost::variant<Find,Calculated,Finalize> Message;
};

class FindStereoFeaturesActor : public Actor<FindStereoFeaturesMessage::Message>
{
public:
	FindStereoFeaturesActor( void );
	~FindStereoFeaturesActor( void );

	const int* getState( void )const;

	void finalize( void );

private:
	struct Impl;
	std::unique_ptr<Impl> mImpl;

	void processMessage( std::shared_ptr<FindStereoFeaturesMessage::Message> msg );
};
