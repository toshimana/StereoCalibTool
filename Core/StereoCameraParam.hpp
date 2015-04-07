#pragma once

#include <memory>

struct StereoCameraParam;
typedef std::shared_ptr<StereoCameraParam> SpStereoCameraParam;

struct StereoCameraParam
{
#define DECLARATION(type,name,tag) type name; const std::string TAG_##name = #tag;

	DECLARATION( cv::Size, lImageSize, imageSizeL );
	DECLARATION( cv::Mat, lCameraMatrix, cameraMatrixL );
	DECLARATION( cv::Mat, lDistCoeffs,   distCoeffsL );

	DECLARATION( cv::Size, rImageSize, imageSizeR );
	DECLARATION( cv::Mat, rCameraMatrix, cameraMatrixR );
	DECLARATION( cv::Mat, rDistCoeffs,   distCoeffsR );

	DECLARATION( cv::Mat, R, rot );
	DECLARATION( cv::Mat, T, trans );

	DECLARATION( double, err, reprojectError );

#undef declaration

	int write( const std::string& path ) const
	{
		cv::FileStorage fs( path, cv::FileStorage::WRITE );
		if ( !fs.isOpened() ) return -1;

		fs << TAG_lImageSize    << lImageSize;
		fs << TAG_lCameraMatrix << lCameraMatrix;
		fs << TAG_lDistCoeffs   << lDistCoeffs;

		fs << TAG_rImageSize    << rImageSize;
		fs << TAG_rCameraMatrix << rCameraMatrix;
		fs << TAG_rDistCoeffs   << rDistCoeffs;

		fs << TAG_R << R;
		fs << TAG_T << T;

		fs << TAG_err << err;

		return 0;
	}

	int read( const std::string& path )
	{
		cv::FileStorage fs( path, cv::FileStorage::READ );
		if ( !fs.isOpened() ) return -1;
		
		fs[TAG_lImageSize]    >> lImageSize;
		fs[TAG_lCameraMatrix] >> lCameraMatrix;
		fs[TAG_lDistCoeffs]   >> lDistCoeffs;

		fs[TAG_rImageSize]    >> rImageSize;
		fs[TAG_rCameraMatrix] >> rCameraMatrix;
		fs[TAG_rDistCoeffs]   >> rDistCoeffs;

		fs[TAG_R] >> R;
		fs[TAG_T] >> T;

		fs[TAG_err] >> err;

		return 0;
	}

};