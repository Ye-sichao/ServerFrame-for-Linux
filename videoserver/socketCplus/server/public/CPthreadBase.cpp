#include "CPthreadBase.h"

CPthreadBase::CPthreadBase()
:isonly(true),isquit(false)
{
	if(this->isonly)
	{
		if(pthread_create(&this->thread_id,NULL,thread_handler,this)!=0)
		{
			perror("pthread create error:");
		}

	}
}

CPthreadBase::~CPthreadBase()
{

}

void * CPthreadBase::thread_handler( void *arg )
{
	CPthreadBase *p=(CPthreadBase *)arg;
	pthread_detach(p->thread_id);
	while(!p->isquit)
	{
		p->run();
	}
	
}

void CPthreadBase::run()
{

}