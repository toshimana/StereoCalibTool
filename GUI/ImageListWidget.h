#ifndef IMAGELISTWIDGET_H
#define IMAGELISTWIDGET_H

#include <QListWidget>
#include <functional>
#include <memory>

#include <opencv2/core/core.hpp>
#include "CornerInfo.h"

class ImageListWidget : public QListWidget
{
	Q_OBJECT

public:
	typedef std::vector<std::vector<cv::Point3f> > Points3f;
	typedef std::vector<std::vector<cv::Point2f> > Points2f;
	typedef std::shared_ptr<Points3f> SpPoints3f;
	typedef std::shared_ptr<Points2f> SpPoints2f;

	ImageListWidget(QWidget *parent = 0);
	~ImageListWidget();

	void addFeatures( const cv::Mat& leftImage, const CornerInfo& leftInfo, const cv::Mat& rightImage, const CornerInfo& rightInfo );
	void getFeatures( SpPoints3f& objectPoints, SpPoints2f& leftImagePoints, SpPoints2f& rightImagePoints, cv::Size& imageSize ) const;

private:
	struct Impl;
	std::unique_ptr<Impl> mImpl;
};

#endif // IMAGELISTWIDGET_H
