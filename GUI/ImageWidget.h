#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QGraphicsView>
#include <opencv2/core/core.hpp>
#include <functional>
#include <memory>

class ImageWidget : public QGraphicsView
{
    Q_OBJECT
public:
    ImageWidget( QWidget* pWnd );
	virtual ~ImageWidget( void );

    void 
	setImage( cv::InputArray img, const int index = 0 );

	cv::Mat
	getImage( const int index = 0 );

	void
	setScaleIndex( const int index = -1);

	double
	getScale( void ) const;

	void
	connectClickedPoint( std::function<void (const QPoint&)> func );

	void
	connectChangedMouseMove( std::function<void (const QPoint&)> func );

	void
	connectChangedScale( std::function<void (const double)> func );

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
	void wheelEvent( QWheelEvent* event );

private:

	struct Impl;
	std::unique_ptr<Impl> mImpl;
};

#endif // IMAGEWIDGET_H