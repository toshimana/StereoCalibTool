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

	int write( const std::string& path ) const
	{
		cv::FileStorage fs( path, cv::FileStorage::WRITE );
		if ( !fs.isOpened() ) return -1;

		fs << "cameraMatrixL" << lCameraMatrix;
		fs << "distCoeffsL" << lDistCoeffs;

		fs << "cameraMatrixR" << rCameraMatrix;
		fs << "distCoeffsR" << rDistCoeffs;

		fs << "rot" << R;
		fs << "trans" << T;

		fs << "reprojectError" << err;

		return 0;
	}
};