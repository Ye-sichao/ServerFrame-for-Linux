#ifndef PTHREADPOLL_H_
#define PTHREADPOLL_H_

#include <pthread.h>
#include <iostream>
#include <list>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
using namespace std;

class CTaskBase
{
public:
	CTaskBase();
	CTaskBase(int fd,char *buf);
	~CTaskBase();
virtual void Run()=0;
	void Setfd(int fd);
	void Setbuf(char *buf);
	int Getfd();
	char *Getbuf();
protected:
	int fd;
	char buf[1024];


	
};
//线程池类，使用链表来保存任务
class CThreadPoll
{
public:
	CThreadPoll(int initevent=5); //通过参数传入的是初始线程池中的等待线程
	~CThreadPoll();
	void Start();
	bool AddTask(CTaskBase *task);
	bool AddTaskPair();
	static void *thread_fun(void *p);//这个函数用来调用任务。一般一开始是要堵塞，等待唤醒
	list<CTaskBase *> GetTaskList();
private:
	 list<CTaskBase *> task_list;
	int MaxCount;//最大线程数就是要创建的线程池对象中一共有多少个线程可以使用
	int WaitCount;
	int CurCount;//保存当前正在执行的任务数
	pthread_mutex_t mutex_t;
	pthread_cond_t cond_t;
	bool isshutdown;

	



};
#endif