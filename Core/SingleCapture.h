#pragma once

#include <memory>
#include <functional>
#include <opencv2/core/core.hpp>

class SingleCapture
{
public:
	SingleCapture( void );
	~SingleCapture( void );

	int initialize( int device );

	void capture( void );

	void connectCaptureImage( std::function<void( cv::Mat )> func );

private:
	struct Impl;
	std::unique_ptr<Impl> mImpl;
};
