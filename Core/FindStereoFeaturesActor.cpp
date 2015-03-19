#include "FindStereoFeaturesActor.h"

#include <boost/signals2/signal.hpp>

#include "FindFeaturesActor.h"

struct FindStereoFeaturesActor::Impl
{
	class MessageVisitor : public boost::static_visitor < void >
	{
	public:
		MessageVisitor( FindStereoFeaturesActor* const obj ) : base( obj ){}

		void operator()( const FindStereoFeaturesMessage::Find& msg ){

		}
	private:
		FindStereoFeaturesActor* base;
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
