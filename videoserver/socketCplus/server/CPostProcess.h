#ifndef CPOSTPROCESS_H_
#define CPOSTPROCESS_H_

#include <unistd.h>
#include <stdio.h>
#include "ipc/CMemShBase.h"
#include "ipc/CSemBase.h"
#include "pthreadPoll/pthreadPoll.h"
#include <iostream>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include "public/TrPack.h"
#include <map>
using namespace std;
//这个类用于接收包并且处理响应，在这个进程里维护一个在线列表
class CDealTask:public CTaskBase
{
public:
	CDealTask(CSemBase *sem,CMemShBase *rdshm,CMemShBase *wrshm,char *buf,map<int,char*> *onlinelist);
	~CDealTask();
	void Run(); //任务的流程，处理包，完成操作之后返回应答包
	void LoginDeal(DEALPACK dealpck,char *Anspack);
	void VideoRecv(DEALPACK dealpck,char *Anspack);
	void VideoPlay(DEALPACK dealpck,char *Anspack);
private:
	CSemBase *sem;
	CMemShBase *shm;
	CMemShBase *wrshm;
	int tty_fd;
	char buf[1024];
	map<int,char*> *onlinelist;
	//线程锁
	pthread_mutex_t mutex;
};

class CPostProcess
{
public:
	CPostProcess();
	~CPostProcess();
	//对包的处理函数，先处理登录包
	
private:
	CThreadPoll pthreadpoll; //线程池用于处理数据
	CSemBase *postsem;  //信号量执行V操作 
	CMemShBase *postshm; //从共享内存读取解包后的结果
	CMemShBase *Ansshm;   //返回给前置进程应答包的共享内存
	map<int,char*> *onlinelist; //维护一个在线用户列表
	CMemShBase *WriteShm;//用于传输应答包给前置进程的共享内存
	CSemBase *sem;
	
	
};
#endif