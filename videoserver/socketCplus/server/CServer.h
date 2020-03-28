#ifndef CSERVER_H_
#define CSERVER_H_
#include "./public/CTcpEpoll.h"
#include "./public/TrPack.h"
#include "ipc/CMemShBase.h"
#include "ipc/CSemBase.h"
#include "pthreadPoll/pthreadPoll.h"
#include "public/CPthreadBase.h"
#include <signal.h>
using namespace std;
//任务处理类
class CMytask :public CTaskBase
{
public:
	CMytask(int fd,char *buf,CSemBase *sem,CMemShBase * shm);
	~CMytask();
	void Run();
	CSemBase *GetSem();
	CMemShBase *GetMem();
private:
	CSemBase *sem;
	CMemShBase * shm;
};
//单独一条线程用于读取共享内存并且发包
class CSendThread :public CPthreadBase
{
public:
	CSendThread(CSemBase *sem,CMemShBase *rdshm);
	~CSendThread();
	void run();
private:
	CSemBase *sem;
	CMemShBase *shm;
};

class CServer:public CTcpEpoll
{
public:
	CServer(int maxevents);
	~CServer();
	void Ondata(int fd,char *buf);
	CSemBase *GetSem();
	CMemShBase *GetMem();
	static void sighandler(int num);
	void StickPckDeal(char *buf,int fd);
;private:
	CThreadPoll threadpoll;
	CSemBase *sem;
	CMemShBase *shm;
	CMemShBase *rdshm;
	CSemBase *rdsem;
	CSendThread *sendthread;
};
#endif
