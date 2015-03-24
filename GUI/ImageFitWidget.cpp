#include "ImageFitWidget.h"
#include <opencv2/imgproc/imgproc.hpp>

#include <boost/signals2/signal.hpp>

#include <QtDebug>
#include <QMouseEvent>

struct ImageFitWidget::Impl
{
	Impl( ImageFitWidget* obj);
	ImageFitWidget* base;

	boost::signals2::signal<void (const QPoint&)>  changedClickedPointOnImage;
	boost::signals2::signal<void (const QPoint&)>  changedMouseMovePointOnImage;
	boost::signals2::signal<void (const double)>   changedScale;
	boost::signals2::signal<void (const cv::Mat&)> changedImage;

	std::vector<QImage>  displayImages;
	QTransform matrix;
	QPoint* shiftPrePosition;    //平行移動量計算用の位置

	void
	CreateTransformMatrix( void );
};

ImageFitWidget::Impl::Impl( ImageFitWidget* obj )
	: base( obj )
	, shiftPrePosition( NULL )
	, displayImages( 1 )
{

}

// 以下の値が変更されたときに実行する 
// 1.scaleIndex
// 2.displayImage( displayImage->width(), displayImage->height() )
// 3.表示サイズ( ImageWidget::width(), ImageWidget::height() )
void
ImageFitWidget::Impl::CreateTransformMatrix( void )
{
	int imageWidth  = displayImages[0].width();
	int imageHeight = displayImages[0].height();
	double scale = std::min( base->width() / static_cast<double>(imageWidth), base->height() / static_cast<double>(imageHeight) );

	QTransform trans;
	trans.translate( (base->width()-scale*imageWidth)/2, (base->height()-scale*imageHeight)/2 ); // 画像の左上の位置
	trans.scale( scale, scale );
	
	matrix = trans;
	base->viewport()->update();
}

ImageFitWidget::ImageFitWidget( QWidget *pWnd )
	: QGraphicsView( pWnd )
	, mImpl( new ImageFitWidget::Impl(this) )
{
	// マウスをクリックしていない時でも
	// mouseMoveEventが反応するように設定する
	setMouseTracking( true );
}

ImageFitWidget::~ImageFitWidget( void )
{
} 

void
ImageFitWidget::setImage( cv::InputArray src, const int index )
{
	if ( mImpl->displayImages.size() <= index ) {
		mImpl->displayImages.resize( index + 1 );
	}

	cv::Mat rgbaImage;
	switch ( src.channels() ) {
#if OPENCV_VERSION_CODE < OPENCV_VERSION(3,0,0)
	case 1:	cv::cvtColor( src, rgbaImage, CV_GRAY2RGBA ); break;
	case 3:	cv::cvtColor( src, rgbaImage, CV_BGR2RGBA ); break;
	case 4:	cv::cvtColor( src, rgbaImage, CV_BGRA2RGBA ); break;
#else
	case 1:	cv::cvtColor( src, rgbaImage, cv::COLOR_GRAY2RGBA ); break;
	case 3:	cv::cvtColor( src, rgbaImage, cv::COLOR_BGR2RGBA ); break;
	case 4:	cv::cvtColor( src, rgbaImage, cv::COLOR_BGRA2RGBA ); break;
#endif
		default: break;
	}

	mImpl->displayImages[index] = 
		QImage(rgbaImage.ptr(),rgbaImage.cols,rgbaImage.rows,rgbaImage.step,QImage::Format_RGBA8888 ).copy();

	mImpl->CreateTransformMatrix();

	// メイン画像が更新されたなら、接続関数を実行する
	if ( index == 0 ) {
		cv::Mat bgrImage;
#if OPENCV_VERSION_CODE < OPENCV_VERSION(3,0,0)
		cv::cvtColor(rgbaImage, bgrImage, CV_RGBA2BGR);
#else
		cv::cvtColor(rgbaImage, bgrImage, cv::COLOR_RGBA2BGR);
#endif
		mImpl->changedImage( bgrImage );
	}
}

cv::Mat
ImageFitWidget::getImage( const int index )
{
	QImage displayImage = mImpl->displayImages[index];

	cv::Mat rgba(displayImage.height(), displayImage.width(), CV_8UC4,
		displayImage.bits(),
		displayImage.bytesPerLine());

	cv::Mat bgr;
#if OPENCV_VERSION_CODE < OPENCV_VERSION(3,0,0)
	cv::cvtColor( rgba, bgr, CV_RGBA2BGR );
#else
	cv::cvtColor( rgba, bgr, cv::COLOR_RGBA2BGR );
#endif
	return bgr;
}

void
ImageFitWidget::connectClickedPoint( std::function<void (const QPoint&)> func )
{
	mImpl->changedClickedPointOnImage.connect( func );
}

void
ImageFitWidget::connectChangedMouseMove( std::function<void (const QPoint&)> func )
{
	mImpl->changedMouseMovePointOnImage.connect( func );
}

void
ImageFitWidget::connectChangedImage( std::function<void (const cv::Mat&)> func )
{
	mImpl->changedImage.connect( func );
}

void
ImageFitWidget::paintEvent( QPaintEvent* event )
{
	QPainter widgetpainter( viewport() );
	widgetpainter.setTransform( mImpl->matrix );
	for( int i = 0, n = mImpl->displayImages.size(); i < n; ++i ) {
		widgetpainter.drawImage( 0, 0, mImpl->displayImages[i] );
	}
}

void
ImageFitWidget::resizeEvent( QResizeEvent* event )
{
	if ( mImpl->displayImages[0].width() != 0 ) {
		mImpl->CreateTransformMatrix();
	}
}

void
ImageFitWidget::mousePressEvent( QMouseEvent* event )
{
	switch ( event->button() ) {
	case Qt::RightButton: 
		{
			if ( mImpl->shiftPrePosition != NULL ) delete mImpl->shiftPrePosition;
			mImpl->shiftPrePosition = new QPoint( event->pos() );
		} break;
	case Qt::LeftButton:
		{
			qreal postX, postY;
			mImpl->matrix.inverted().map( event->x(), event->y(), &postX, &postY );
			mImpl->changedClickedPointOnImage( QPoint(floor(postX), floor(postY) ) );
		} break;
	}
}

void 
ImageFitWidget::mouseReleaseEvent( QMouseEvent* event )
{
	if ( event->button() == Qt::RightButton ) {
		if ( mImpl->shiftPrePosition != NULL ) delete mImpl->shiftPrePosition;
		mImpl->shiftPrePosition = NULL;
	}

}

void
ImageFitWidget::mouseMoveEvent( QMouseEvent* event )
{

	if ( mImpl->displayImages[0].width() != 0 ) {
		// 画像上の座標を出力する
		qreal postX, postY;
		mImpl->matrix.inverted().map( event->x(), event->y(), &postX, &postY );
		mImpl->changedMouseMovePointOnImage( QPoint(floor(postX), floor(postY) ) );
	}
}

