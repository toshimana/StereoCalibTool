#pragma once

#include <memory>
#include <functional>
#include <opencv2/core/core.hpp>

#include <Actor.hpp>

namespace FindStereoFeaturesMessage
{
	struct Find
	{
		Find( const cv::Mat& left, const cv::Mat& right ) 
			: leftImage( left ), rightImage( right ){}
		const cv::Mat leftImage;
		const cv::Mat rightImage;
	};

	typedef boost::variant<Find> Message;
};

class FindStereoFeaturesActor : public Actor<FindStereoFeaturesMessage::Message>
{
public:
	FindStereoFeaturesActor( void );
	~FindStereoFeaturesActor( void );

private:
	struct Impl;
	std::unique_ptr<Impl> mImpl;

	void processMessage( std::shared_ptr<FindStereoFeaturesMessage::Message> msg );
};
