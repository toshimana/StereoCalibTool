#ifndef TASKSCHEDULER_H
#define TASKSCHEDULER_H

#ifdef __linux__
#define TASKSCHEDULER_API // do nothing.
#else // __linux__

#ifdef TASKSCHEDULER_EXPORTS
#define TASKSCHEDULER_API __declspec(dllexport)
#else 
#define TASKSCHEDULER_API __declspec(dllimport)
#endif 

#endif // __linux__

#include <functional>

class TASKSCHEDULER_API 
TaskScheduler 
{
public:
	TaskScheduler( const unsigned int threadNum );

	~TaskScheduler(void);

	void entry(std::function<void(void)> func);

	void exec( void );
	
private:
  struct Impl;
  Impl*  mImpl;
};

#endif
