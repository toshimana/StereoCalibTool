#pragma once

#include <memory>
#include <functional>
#include <opencv2/core/core.hpp>

#include <Actor.hpp>

namespace FindFeaturesMessage
{
	struct Find
	{
		Find( const cv::Mat& _image, std::function<void( std::shared_ptr<std::vector<cv::Point2f> > )> func )
			: image( _image ), deligate( func ){}
		const cv::Mat image;
		std::function<void( std::shared_ptr<std::vector<cv::Point2f> > )> deligate;
	};

	typedef boost::variant<Find> Message;
};

class FindFeaturesActor : public Actor<FindFeaturesMessage::Message>
{
public:
	FindFeaturesActor( void );
	~FindFeaturesActor( void );

private:
	struct Impl;
	std::unique_ptr<Impl> mImpl;

	void processMessage( std::shared_ptr<FindFeaturesMessage::Message> msg );
};
