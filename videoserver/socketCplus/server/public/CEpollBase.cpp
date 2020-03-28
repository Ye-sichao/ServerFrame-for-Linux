#include "CEpollBase.h"
CEpollBase::CEpollBase( int maxevents )
:isquit(false)
{
	this->maxevents=maxevents;
	//先创建epoll描述字
	this->epfd=epoll_create(20);
	if(this->epfd==-1)
	{
		perror("epoll create fail\n");
		exit(0);
	}
	//初始化最大监听数组
	memset(&this->event,0,sizeof(struct epoll_event));
	this->res_event=new struct epoll_event[maxevents];
	memset(this->res_event,0,sizeof(struct epoll_event)*maxevents);


}
CEpollBase::~CEpollBase()
{
	delete []res_event;
}
bool CEpollBase::AddEvent( int fd,int eventtype)
{
	int res;
	this->event.events=eventtype;
	this->event.data.fd=fd;
	res=epoll_ctl(this->epfd,EPOLL_CTL_ADD,fd,&this->event);
	if(res==-1)
	{
		perror("event add fail:\n");
	}
}

bool CEpollBase::DelEvent( int fd,int eventtype)
{
	int res;
	this->event.events=eventtype;
	this->event.data.fd=fd;
	res=epoll_ctl(this->epfd,EPOLL_CTL_DEL,fd,&this->event);
	if(res==-1)
	{
		perror("event delete fail:\n");
	}
}

bool CEpollBase::ModEvent( int fd,int eventtype)
{
	int res;
	this->event.events=eventtype;
	this->event.data.fd=fd;
	res=epoll_ctl(this->epfd,EPOLL_CTL_MOD,fd,&this->event);
	if(res==-1)
	{
		perror("event modify fail:\n");
	}
}

void CEpollBase::Start()
{
	int res;
	while(!isquit)
	{
		cout<<"epoll wait......."<<endl;
		res=epoll_wait(epfd,res_event,maxevents,-1);
		if(res<0)
		{
			perror("epoll wait error:\n");
		}
		else if(res==0)
		{
			perror("epoll timeout:\n");
		}
		else
		{
			cout<<"Run"<<endl;
			this->Run(res);
		}

	}
}

void CEpollBase::Stop()
{
	this->isquit=true;
}

