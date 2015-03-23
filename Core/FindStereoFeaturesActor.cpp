#include "FindStereoFeaturesActor.h"

#include <boost/signals2/signal.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>

#include "FindFeaturesActor.h"

#include "WaitSync.hpp"

struct FindStereoFeaturesActor::Impl
{
	struct Machine_ : boost::msm::front::state_machine_def < Machine_ >
	{
		Machine_(FindStereoFeaturesActor* obj):base(obj){}

		// State
		struct WaitState : boost::msm::front::state < > {};
		struct ProcState : boost::msm::front::state < > {};
		struct HaltState : boost::msm::front::state < > {};

		// Function
		void on_find( const FindStereoFeaturesMessage::Find& msg )
		{
			using namespace FindStereoFeaturesMessage;

			typedef WaitSync<CornerInfo, CornerInfo> WaitStereoCornerInfo;
			std::shared_ptr<WaitStereoCornerInfo> ws = std::make_shared<WaitStereoCornerInfo>();
			ws->connectSync( std::bind( &FindStereoFeaturesActor::Impl::Machine_::filter, this, std::placeholders::_1, std::placeholders::_2, msg.deligate ) );
			base->mImpl-> leftFindFeaturesActor.entry( FindFeaturesMessage::Find( msg. leftImage, [ws]( CornerInfo cornerInfo ){ ws->setFirst ( cornerInfo ); } ) );
			base->mImpl->rightFindFeaturesActor.entry( FindFeaturesMessage::Find( msg.rightImage, [ws]( CornerInfo cornerInfo ){ ws->setSecond( cornerInfo ); } ) );
		}

		// Transition
		struct transition_table : boost::mpl::vector<
			a_row < WaitState, FindStereoFeaturesMessage::Find,       ProcState, &on_find >,
			 _row < ProcState, FindStereoFeaturesMessage::Calculated, WaitState>,
			 _row < WaitState, FindStereoFeaturesMessage::Finalize,   HaltState>,
			 _row < ProcState, FindStereoFeaturesMessage::Finalize,   HaltState>
			> {};

		typedef WaitState initial_state;

	protected:
		template <class FSM, class Event>
		void no_transition( Event const&, FSM&, int )
		{
			// do nothing.
		}


	private:
		FindStereoFeaturesActor* base;

		void filter( CornerInfo left, CornerInfo right, std::function<void(CornerInfo,CornerInfo)> deligate )
		{
			if ( left.isEnable() && right.isEnable() ) {
				deligate( left, right );
				this->base->entry( FindStereoFeaturesMessage::Calculated() );
			}
			else {
				if ( left .isEnable() ) std::cout << "Left  Features Found." << std::endl;
				if ( right.isEnable() ) std::cout << "Right Features Found." << std::endl;
			}
		}
	};

	typedef boost::msm::back::state_machine<Machine_> Machine;

	class MessageVisitor : public boost::static_visitor < void >
	{
	public:
		MessageVisitor( FindStereoFeaturesActor* const obj ) : base( obj ){}

		template<typename T>
		void operator()( const T& msg ){
			base->mImpl->machine.process_event( msg );
		}
	private:
		FindStereoFeaturesActor* base;

	};

	Impl( FindStereoFeaturesActor* const obj)
		: machine( obj )
	{
		machine.start();
	}

	~Impl(){
	}

	Machine machine;

	FindFeaturesActor  leftFindFeaturesActor;
	FindFeaturesActor rightFindFeaturesActor;
};

FindStereoFeaturesActor::FindStereoFeaturesActor( void )
	:mImpl(new FindStereoFeaturesActor::Impl( this ))
{}

FindStereoFeaturesActor::~FindStereoFeaturesActor( void )
{
}

const int*
FindStereoFeaturesActor::getState( void )const
{
	return mImpl->machine.current_state();
}

void
FindStereoFeaturesActor::finalize( void )
{
	entry( FindStereoFeaturesMessage::Finalize() );
	while ( *getState() != 2 ) {
		boost::this_thread::sleep_for( boost::chrono::milliseconds( 1 ) );
	}
}

void 
FindStereoFeaturesActor::processMessage( std::shared_ptr<FindStereoFeaturesMessage::Message> msg )
{
	Impl::MessageVisitor mv( this );
	boost::apply_visitor( mv, *msg );
}
