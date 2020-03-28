#include "pthreadPoll.h"
//����������ʱ��鿴��û�еȴ����̣߳�û�����ж��ܲ��ܿ������߳�
CThreadPoll::CThreadPoll( int initevent )
{
	//�̳߳س�ʼ״̬
	this->isshutdown=false;
	//��ʼ�����������ͻ�����
	pthread_mutex_init(&this->mutex_t,NULL);
	pthread_cond_init(&this->cond_t,NULL);
	this->MaxCount=32;   //�����̳߳ص�����߳���
	this->WaitCount=initevent;
	this->CurCount=initevent;
	if(initevent>this->MaxCount)
	{
		cout<<"cannot create so lots pthread"<<endl;
	}

	//ѭ��������Ӧ�ĵȴ��߳�
	int i=0;
	pthread_t thread_id;
	for(;i<initevent;i++)
	{
		pthread_create(&thread_id,NULL,thread_fun,this);
	}

}
void * CThreadPoll::thread_fun( void *p )
{
	//��ʼ���̵߳ĵȴ�ʱ��
	struct timeval now;
	struct timespec outtime;
	int error_id;
	gettimeofday(&now,NULL);
	outtime.tv_sec = now.tv_sec+10;
	outtime.tv_nsec = (now.tv_usec+1000UL*5)*1000UL;
	CThreadPoll *poll=(CThreadPoll *)p;
	//�߳�һ���룬���������߳����룬�߳����꣬�Լ��Զ��ͷ�
	pthread_detach(pthread_self());
	while(poll->isshutdown==false)
	{
		//ÿһ���߳̽���֮�󣬶�����һ�λ�������������ס����߳�
		pthread_mutex_lock(&(poll->mutex_t));
		if(poll->task_list.empty())
		{
			//���û��������ô�͵ȴ�
			cout<<"pthread wait .......\n"<<endl;
			cout<<"wait Count= "<<poll->WaitCount<<"cur count= "<< poll->CurCount<<endl;
			error_id=pthread_cond_wait(&(poll->cond_t),&(poll->mutex_t));//˯�ߵȴ��������  ʹ�ó�ʱ�ȴ����������ʱ�����������߳�
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
			//�������ִ������Ĵ����
			//ÿһ���߳���ִ�еĴ��룬���Ǳ����¼����У�Ȼ��ȡ�����������ִ��
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
	this->task_list.push_back(task);//�������������б���
	pthread_mutex_unlock(&this->mutex_t);
	//�ж��̳߳�����û�еȴ��߳�
	//cout<<"waitcout= "<<this->WaitCount<<"curcount= "<<this->CurCount<<endl;
	if(this->WaitCount>0&&this->CurCount!=0)
	{
		cout<<"wake up a pthread"<<endl;
		pthread_cond_signal(&(this->cond_t));   //����һ���߳������������
	}
	else if(this->CurCount<this->MaxCount)
	{
		cout<<"create a new pthread"<<endl;
		pthread_create(&pthread_id,NULL,thread_fun,this);  //�����ǰ�߳���û�д��������߳�������������������߳�
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