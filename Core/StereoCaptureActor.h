#pragma once

#include <memory>
#include <functional>
#include <opencv2/core/core.hpp>

#include <Actor.hpp>

#ifdef __linux__
#define STEREOCAPTUREACTOR_API // do nothing. 
#else // __linux__
#ifdef STEREOCAPTUREACTOR_EXPORTS
#define STEREOCAPTUREACTOR_API __declspec(dllexport)
#else
#define STEREOCAPTUREACTOR_API __declspec(dllimport)
#endif
#endif

namespace StereoCaptureMessage
{
	struct Initialize
	{
		Initialize( int _leftDevice, int _rightDevice )
			: leftDevice( _leftDevice ), rightDevice( _rightDevice ){}
		int leftDevice;
		int rightDevice;
	};

	struct Capture
	{
	};

	typedef boost::variant<Initialize, Capture> Message;
};

class StereoCaptureActor : public Actor<StereoCaptureMessage::Message>
{
public:
	STEREOCAPTUREACTOR_API StereoCaptureActor( void );
	STEREOCAPTUREACTOR_API ~StereoCaptureActor( void );

	STEREOCAPTUREACTOR_API void connectCaptureImage( std::function<void(cv::Mat,cv::Mat)> func );

private:
	struct Impl;
	std::unique_ptr<Impl> mImpl;

	void processMessage( std::shared_ptr<StereoCaptureMessage::Message> msg );
};
