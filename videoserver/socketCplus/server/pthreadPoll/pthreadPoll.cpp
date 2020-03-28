#include "pthreadPoll.h"
//在添加任务的时候查看有没有等待的线程，没有在判断能不能开启新线程
CThreadPoll::CThreadPoll( int initevent )
{
	//线程池初始状态
	this->isshutdown=false;
	//初始化条件变量和互斥量
	pthread_mutex_init(&this->mutex_t,NULL);
	pthread_cond_init(&this->cond_t,NULL);
	this->MaxCount=32;   //设置线程池的最大线程数
	this->WaitCount=initevent;
	this->CurCount=initevent;
	if(initevent>this->MaxCount)
	{
		cout<<"cannot create so lots pthread"<<endl;
	}

	//循环创建对应的等待线程
	int i=0;
	pthread_t thread_id;
	for(;i<initevent;i++)
	{
		pthread_create(&thread_id,NULL,thread_fun,this);
	}

}
void * CThreadPoll::thread_fun( void *p )
{
	//初始化线程的等待时间
	struct timeval now;
	struct timespec outtime;
	int error_id;
	gettimeofday(&now,NULL);
	outtime.tv_sec = now.tv_sec+10;
	outtime.tv_nsec = (now.tv_usec+1000UL*5)*1000UL;
	CThreadPoll *poll=(CThreadPoll *)p;
	//线程一进入，就先与主线程脱离，线程走完，自己自动释放
	pthread_detach(pthread_self());
	while(poll->isshutdown==false)
	{
		//每一条线程进入之后，都调用一次互斥量，来阻塞住这个线程
		pthread_mutex_lock(&(poll->mutex_t));
		if(poll->task_list.empty())
		{
			//如果没有任务，那么就等待
			cout<<"pthread wait .......\n"<<endl;
			cout<<"wait Count= "<<poll->WaitCount<<"cur count= "<< poll->CurCount<<endl;
			error_id=pthread_cond_wait(&(poll->cond_t),&(poll->mutex_t));//睡眠等待任务进来  使用超时等待，超过这个时间就销毁这个线程
			cout<<"start task"<<endl;
			pthread_mutex_unlock(&((CThreadPoll *)p)->mutex_t);
			/*if(error_id==ETIMEDOUT)
			{
				//if(poll->WaitCount>1&&poll->CurCount>1)
				//{
					cout<<"delete a pthread"<<endl;
					pthread_mutex_lock(&(poll->mutex_t));
					poll->WaitCount--;
					poll->CurCount--;
					pthread_mutex_unlock(&(poll->mutex_t));
					pthread_mutex_unlock(&(poll->mutex_t));
					pthread_exit(NULL);
				}
				else
				{

				}
			
			}
			else
			{
				perror("timeout error:");
				pthread_mutex_unlock(&(poll->mutex_t));
			}
			pthread_mutex_unlock(&(poll->mutex_t));*/
		
			
		}
		else
		{
			//这里就是执行任务的代码段
			//每一个线程所执行的代码，都是遍历事件队列，然后取出这个任务来执行
			CTaskBase *Task;
			list<CTaskBase *>:: iterator it;
			it=((CThreadPoll *)p)->task_list.begin();
			Task=*it;
			((CThreadPoll *)p)->task_list.erase(it); 
			poll->WaitCount--;
			cout<<"will run"<<endl;
			Task->Run();
			poll->WaitCount++;
			pthread_mutex_unlock(&((CThreadPoll *)p)->mutex_t);
			
		}		

	}
		
}

bool CThreadPoll::AddTask( CTaskBase *task )
{
	cout<<"Add task"<<endl;
	pthread_t pthread_id;
	pthread_mutex_lock(&this->mutex_t);
	this->task_list.push_back(task);//把任务加入这个列表中
	pthread_mutex_unlock(&this->mutex_t);
	//判断线程池中有没有等待线程
	//cout<<"waitcout= "<<this->WaitCount<<"curcount= "<<this->CurCount<<endl;
	if(this->WaitCount>0&&this->CurCount!=0)
	{
		cout<<"wake up a pthread"<<endl;
		pthread_cond_signal(&(this->cond_t));   //唤醒一条线程来处理该任务
	}
	else if(this->CurCount<this->MaxCount)
	{
		cout<<"create a new pthread"<<endl;
		pthread_create(&pthread_id,NULL,thread_fun,this);  //如果当前线程数没有大于最大的线程数，就允许继续开辟线程
		this->CurCount++;
		this->WaitCount++;
	}
}

CThreadPoll::~CThreadPoll()
{

}
CTaskBase::CTaskBase(int fd,char *buf)
:fd(fd)
{
	memset(this->buf,0,1024);
	memcpy(this->buf,buf,1024);
}

CTaskBase::CTaskBase()
{

}
CTaskBase::~CTaskBase()
{

}

void CTaskBase::Setfd( int fd )
{
	this->fd=fd;

}

void CTaskBase::Setbuf( char *buf )
{
	memcpy(this->buf,buf,1024);
}

int CTaskBase::Getfd()
{
	return this->fd;
}

char * CTaskBase::Getbuf()
{
	return this->buf;
}