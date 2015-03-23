#pragma once

#include <functional>

#include <Actor.hpp>
#include "CornerInfo.h"

namespace FindFeaturesMessage
{
	struct Find
	{
		Find( const cv::Mat& _image, std::function<void( CornerInfo )> func )
			: image( _image ), deligate( func ){}
		const cv::Mat image;
		std::function<void(CornerInfo)> deligate;
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
