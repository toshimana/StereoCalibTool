#pragma once

#include <ActorBase.hpp>

namespace MainActorMessage
{
	struct ExecFunc
	{
		ExecFunc( std::function<void( void )> _func ) : func( _func ){}
		std::function<void( void )> func;
	};
	typedef boost::variant<ExecFunc> Message;
};

struct MainActor : public ActorBase < MainActorMessage::Message >
{
private:
	void processMessage( std::shared_ptr<MainActorMessage::Message> msg )
	{
		MessageVisitor mv( this );
		boost::apply_visitor( mv, *msg );
	}

	class MessageVisitor : public boost::static_visitor < void >
	{
	public:
		MessageVisitor( MainActor* const obj ) : base( obj ){}

		void operator()( const MainActorMessage::ExecFunc& msg )
		{
			msg.func();
		}

	private:
		MainActor* const base;
	};
};