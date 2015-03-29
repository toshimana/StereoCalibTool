#include "ImageListWidget.h"
#include <QList>
#include <boost/signals2/signal.hpp>
#include <boost/format.hpp>


struct ImageListWidget::Impl
{
	Impl( ImageListWidget* obj )
		: objectPoints(std::make_shared<Points3f>())
		, leftImagePoints(std::make_shared<Points2f>())
		, rightImagePoints(std::make_shared<Points2f>())
	{}

	boost::signals2::signal<void(const std::string&)> changedCurrentText;

	SpPoints3f objectPoints;
	SpPoints2f leftImagePoints;
	SpPoints2f rightImagePoints;
	cv::Size   imageSize;

	void addPoints( const CornerInfo::SpCorners leftCorners, const CornerInfo::SpCorners rightCorners ) 
	{
		std::vector<cv::Point3f> ops;
		std::vector<cv::Point2f> lps, rps;

		for ( int row = 0; row < 8; ++row ){
			for ( int col = 0; col < 13; ++col ) {
				ops.push_back( cv::Point3f( 20.0*col, 20.0*row, 0.0 ) );
			}
		}
		std::copy(  leftCorners->begin(),  leftCorners->end(), std::back_inserter( lps ) );
		std::copy( rightCorners->begin(), rightCorners->end(), std::back_inserter( rps ) );

		objectPoints    ->push_back( ops );
		leftImagePoints ->push_back( lps );
		rightImagePoints->push_back( rps );
	}
};

ImageListWidget::ImageListWidget(QWidget *parent)
	: QListWidget(parent)
	, mImpl( std::make_unique<ImageListWidget::Impl>( this ) )
{
}

ImageListWidget::~ImageListWidget()
{
}

void
ImageListWidget::addFeatures( const cv::Mat& leftImage, const CornerInfo& leftInfo, const cv::Mat& rightImage, const CornerInfo& rightInfo )
{
	// TODO: store features.

	static int count = 0;
	std::string cStr = (boost::format( "%03d" ) % ++count).str();
	addItem( cStr.c_str() );

	mImpl->addPoints( leftInfo.corners, rightInfo.corners );
	mImpl->imageSize = leftImage.size();
}

void 
ImageListWidget::getFeatures( SpPoints3f& objectPoints, SpPoints2f& leftImagePoints, SpPoints2f& rightImagePoints, cv::Size& imageSize ) const
{
	objectPoints     = mImpl->objectPoints;
	leftImagePoints  = mImpl->leftImagePoints;
	rightImagePoints = mImpl->rightImagePoints;
	imageSize        = mImpl->imageSize;
}