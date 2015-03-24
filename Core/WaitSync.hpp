#pragma once

#include <functional>
#include <boost/optional.hpp>
#include <boost/signals2/signal.hpp>

template<typename T, typename U>
class WaitSync
{
public:
	WaitSync( void ) : first(), second() {}

	void setFirst( const T& obj ){ 
		boost::mutex::scoped_lock lock( mutex );
		first = obj; run(); 
	}
	void setSecond( const U& obj ){	
		boost::mutex::scoped_lock lock( mutex );
		second = obj; run(); 
	}

	void connectSync( std::function<void( T, U )> func ){ sync.connect( func ); }

private:
	boost::optional<T> first;
	boost::optional<U> second;

	boost::signals2::signal<void( T, U )> sync;
	boost::mutex mutex;

	void reset( void ){ first = boost::none; second = boost::none; }

	void run( void ) {
		if ( first && second ) {
			T fst = *first;
			U snd = *second;
			reset();
			sync( fst, snd );
		}
	}
};