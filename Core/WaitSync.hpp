#pragma once

#include <functional>
#include <boost/optional.hpp>
#include <boost/signals2/signal.hpp>

template<typename T, typename U>
class WaitSync
{
public:
	WaitSync( void ) : first(), second() {}

	void setFirst( const T& obj ){ first = obj; run(); }
	void setSecond( const U& obj ){	second = obj; run(); }

	void connectSync( std::function<void( T, U )> func ){ sync.connect( func ); }

private:
	boost::optional<T> first;
	boost::optional<U> second;

	boost::signals2::signal<void( T, U )> sync;

	void reset( void ){ first = boost::none; second = boost::none; }

	void run( void ) {
		if ( first && second ) {
			sync( *first, *second );
			reset();
		}
	}
};