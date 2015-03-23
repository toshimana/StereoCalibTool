#include "FindFeaturesActor.h"

#include <boost/signals2/signal.hpp>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include "FindFeaturesActor.h"

struct FindFeaturesActor::Impl
{
	class MessageVisitor : public boost::static_visitor < void >
	{
	public:
		MessageVisitor( FindFeaturesActor* const obj ) : base( obj ){}

		void operator()( const FindFeaturesMessage::Find& msg ){
			cv::Size patternSize( 12, 8 );

			std::shared_ptr<std::vector<cv::Point2f> > corners = std::make_shared<std::vector<cv::Point2f> >();
			bool patternFound = 
				cv::findChessboardCorners( msg.image, patternSize, *corners,
				cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE );

			if ( patternFound ) {
				cv::cornerSubPix( msg.image, *corners, cv::Size( 5, 5 ), cv::Size( -1, -1 ),
					cv::TermCriteria( cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 20, 1.0 ) );
			}

			msg.deligate( CornerInfo( patternSize, corners, patternFound ) );
		}
	private:
		FindFeaturesActor* base;
	};

	Impl( FindFeaturesActor* const obj)
	{
	}

	~Impl(){
	}
};

FindFeaturesActor::FindFeaturesActor( void )
	:mImpl(new FindFeaturesActor::Impl( this ))
{}

FindFeaturesActor::~FindFeaturesActor( void )
{
}

void 
FindFeaturesActor::processMessage( std::shared_ptr<FindFeaturesMessage::Message> msg )
{
	Impl::MessageVisitor mv( this );
	boost::apply_visitor( mv, *msg );
}
