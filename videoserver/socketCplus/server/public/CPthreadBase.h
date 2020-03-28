#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
//本类用于初始化一条线程，并且提供接口
class CPthreadBase
{
public:
	CPthreadBase();
	~CPthreadBase();
	static void *thread_handler(void *arg);
	virtual void run();
protected:
	pthread_t thread_id;
	bool isonly;
	bool isquit;
};