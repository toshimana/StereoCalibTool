#include "TaskScheduler.h"

#include <queue>
#include <iostream>


#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <memory>

const int MAX_QUEUE_SIZE = 128;

struct TaskScheduler::Impl {
  Impl( void );
  ~Impl( void );
  boost::lockfree::queue<std::function<void(void)>*> entries;
  boost::thread_group threads;

  void exec( void );
  void threadExec( void );
};

TaskScheduler::Impl::Impl( void )
	: entries( MAX_QUEUE_SIZE )
{ }

TaskScheduler::Impl::~Impl( void )
{
	// ƒXƒŒƒbƒh‚Ì’âŽ~
	threads.interrupt_all();
	threads.join_all();
}

TaskScheduler::TaskScheduler( const unsigned int threadNum )
  : mImpl(new TaskScheduler::Impl())
{
  for(int i = 0; i < threadNum; ++i) {
      mImpl->threads.create_thread(std::bind(&TaskScheduler::Impl::threadExec, mImpl));
  }
}

void
TaskScheduler::Impl::exec( void )
{
	std::function<void(void)>* pfunc;
	if ( !entries.pop( pfunc ) ) {
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		return;
	}
	(*pfunc)();
	delete pfunc;
}

void
TaskScheduler::Impl::threadExec( void )
{
  while( true ) {
	  exec();
  }
}


TaskScheduler::~TaskScheduler(void)
{
  delete mImpl;
}

void TaskScheduler::entry(std::function<void(void)> func)
{
  std::function<void(void)>* p = new std::function<void(void)>( func );
  while ( !mImpl->entries.push( p ) ) {
	  boost::this_thread::sleep(boost::posix_time::milliseconds(1));
  };
}

void
TaskScheduler::exec( void )
{
	mImpl->exec();
}
