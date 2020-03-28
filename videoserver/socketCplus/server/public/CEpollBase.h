#include <sys/epoll.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
class CEpollBase
{
public:
	CEpollBase(int maxevents);    //最大的监听数量由使用者指定
	~CEpollBase();
	bool AddEvent(int fd,int eventtype=EPOLLIN);
	bool DelEvent(int fd,int eventtype=EPOLLIN);
	bool ModEvent(int fd,int eventtype=EPOLLIN);
	void Start();
	void Stop();
	virtual void Run(int ncount)=0;	
protected:
	int epfd; //构造里面创建出epoll描述字
	struct epoll_event event;
	struct epoll_event *res_event;
	int maxevents;
	bool isquit;
private:
};