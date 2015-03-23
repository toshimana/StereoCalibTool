#pragma once

#include <vector>
#include <memory>
#include <opencv2/core/core.hpp>

struct CornerInfo
{
	typedef std::vector<cv::Point2f> Corners;
	typedef std::shared_ptr<Corners> SpCorners;

	CornerInfo( const cv::Size& _patternSize, SpCorners _corners, bool _patternWasFound )
		: patternSize( _patternSize ), corners( _corners ), patternWasFound( _patternWasFound ){}

	cv::Size  patternSize;
	SpCorners corners;
	bool      patternWasFound;

	bool isEnable( void ) const
	{
		return ((!corners->empty()) && patternWasFound);
	}
};

