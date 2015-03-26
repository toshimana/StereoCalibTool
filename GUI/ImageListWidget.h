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
	ImageListWidget(QWidget *parent = 0);
	~ImageListWidget();

	void addFeatures( const cv::Mat& leftImage, const CornerInfo& leftInfo, const cv::Mat& rightImage, const CornerInfo& rightInfo );

private:
	struct Impl;
	std::unique_ptr<Impl> mImpl;
};

#endif // IMAGELISTWIDGET_H
