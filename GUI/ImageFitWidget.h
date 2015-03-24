#ifndef IMAGEFITWIDGET_H
#define IMAGEFITWIDGET_H

#include <QGraphicsView>
#include <opencv2/core/core.hpp>
#include <functional>
#include <memory>

class ImageFitWidget : public QGraphicsView
{
    Q_OBJECT
public:
    ImageFitWidget( QWidget* pWnd );
	virtual ~ImageFitWidget( void );

    void 
	setImage( cv::InputArray img, const int index = 0 );

	cv::Mat
	getImage( const int index = 0 );

	void
	connectClickedPoint( std::function<void (const QPoint&)> func );

	void
	connectChangedMouseMove( std::function<void (const QPoint&)> func );

	void
	connectChangedImage( std::function<void (const cv::Mat&)> func );

signals:

public slots:

protected:
    void paintEvent( QPaintEvent* event );
	void resizeEvent( QResizeEvent* event );
	void mousePressEvent( QMouseEvent* event );
	void mouseReleaseEvent( QMouseEvent* event );
	void mouseMoveEvent( QMouseEvent* event );

private:

	struct Impl;
	std::unique_ptr<Impl> mImpl;
};

#endif // IMAGEFITWIDGET_H