#include "ObservedPushButton.h"

#include <boost/signals2/signal.hpp>

struct ObservedPushButton::Impl
{
	boost::signals2::signal<void( void )> pressed;
};

ObservedPushButton::ObservedPushButton( QWidget* parent )
	: QPushButton( parent )
	, mImpl( std::make_unique<ObservedPushButton::Impl>() )
{
	connect( this, SIGNAL( pressed(void) ), this, SLOT( on_pressed(void) ) );
}


ObservedPushButton::~ObservedPushButton()
{
}

void
ObservedPushButton::connectPressed( std::function<void( void )> func )
{
	mImpl->pressed.connect( func );
}

void
ObservedPushButton::on_pressed( void )
{
	mImpl->pressed();
}