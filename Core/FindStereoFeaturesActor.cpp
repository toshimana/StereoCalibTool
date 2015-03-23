#include "FindStereoFeaturesActor.h"

#include <boost/signals2/signal.hpp>

#include "FindFeaturesActor.h"

#include "WaitSync.hpp"

struct FindStereoFeaturesActor::Impl
{
	class MessageVisitor : public boost::static_visitor < void >
	{
	public:
		MessageVisitor( FindStereoFeaturesActor* const obj ) : base( obj ){}

		void operator()( const FindStereoFeaturesMessage::Find& msg ){
			using namespace FindStereoFeaturesMessage;

			typedef WaitSync<CornerInfo, CornerInfo> WaitStereoCornerInfo;
			std::shared_ptr<WaitStereoCornerInfo> ws = std::make_shared<WaitStereoCornerInfo>();
			ws->connectSync( std::bind( &FindStereoFeaturesActor::Impl::MessageVisitor::filter, this, std::placeholders::_1, std::placeholders::_2, msg.deligate ) );
			base->mImpl-> leftFindFeaturesActor.entry( FindFeaturesMessage::Find( msg. leftImage, [ws]( CornerInfo cornerInfo ){ ws->setFirst ( cornerInfo ); } ) );
			base->mImpl->rightFindFeaturesActor.entry( FindFeaturesMessage::Find( msg.rightImage, [ws]( CornerInfo cornerInfo ){ ws->setSecond( cornerInfo ); } ) );
		}
	private:
		FindStereoFeaturesActor* base;

		void filter( CornerInfo left, CornerInfo right, std::function<void(CornerInfo,CornerInfo)> deligate )
		{
			if ( left.isEnable() && right.isEnable() ) {
				deligate( left, right );
			}
		}
	};

	Impl( FindStereoFeaturesActor* const obj)
	{
	}

	~Impl(){
	}

	FindFeaturesActor  leftFindFeaturesActor;
	FindFeaturesActor rightFindFeaturesActor;
};

FindStereoFeaturesActor::FindStereoFeaturesActor( void )
	:mImpl(new FindStereoFeaturesActor::Impl( this ))
{}

FindStereoFeaturesActor::~FindStereoFeaturesActor( void )
{
}

void 
FindStereoFeaturesActor::processMessage( std::shared_ptr<FindStereoFeaturesMessage::Message> msg )
{
	Impl::MessageVisitor mv( this );
	boost::apply_visitor( mv, *msg );
}
