#include "ImageListWidget.h"
#include <QList>
#include <boost/signals2/signal.hpp>
#include <boost/format.hpp>


struct ImageListWidget::Impl
{
	Impl( ImageListWidget* obj );

	boost::signals2::signal<void(const std::string&)> changedCurrentText;
};

ImageListWidget::Impl::Impl( ImageListWidget* obj )
{
}

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
}
