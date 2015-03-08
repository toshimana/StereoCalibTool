#pragma once

#include <memory>
#include <functional>
#include <opencv2/core/core.hpp>

#ifdef __linux__
#define STEREOCAPTURE_API // do nothing. 
#else // __linux__
#ifdef STEREOCAPTURE_EXPORTS
#define STEREOCAPTURE_API __declspec(dllexport)
#else
#define STEREOCAPTURE_API __declspec(dllimport)
#endif
#endif

class StereoCapture
{
public:
	STEREOCAPTURE_API StereoCapture( void );
	STEREOCAPTURE_API ~StereoCapture( void );

	STEREOCAPTURE_API int initialize( void );

	STEREOCAPTURE_API void connectCaptureImage( std::function<void(cv::Mat,cv::Mat)> func );

private:
	struct Impl;
	std::unique_ptr<Impl> mImpl;
};
