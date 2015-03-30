#pragma once

#include <memory>

struct StereoCameraParam;
typedef std::shared_ptr<StereoCameraParam> SpStereoCameraParam;

struct StereoCameraParam
{
	cv::Mat lCameraMatrix;
	cv::Mat lDistCoeffs;

	cv::Mat rCameraMatrix;
	cv::Mat rDistCoeffs;

	cv::Mat R;
	cv::Mat T;

	double err;
};