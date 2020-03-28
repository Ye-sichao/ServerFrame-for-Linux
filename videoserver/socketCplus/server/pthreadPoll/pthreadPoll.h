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
//�̳߳��࣬ʹ����������������
class CThreadPoll
{
public:
	CThreadPoll(int initevent=5); //ͨ������������ǳ�ʼ�̳߳��еĵȴ��߳�
	~CThreadPoll();
	void Start();
	bool AddTask(CTaskBase *task);
	bool AddTaskPair();
	static void *thread_fun(void *p);//�������������������һ��һ��ʼ��Ҫ�������ȴ�����
	list<CTaskBase *> GetTaskList();
private:
	 list<CTaskBase *> task_list;
	int MaxCount;//����߳�������Ҫ�������̳߳ض�����һ���ж��ٸ��߳̿���ʹ��
	int WaitCount;
	int CurCount;//���浱ǰ����ִ�е�������
	pthread_mutex_t mutex_t;
	pthread_cond_t cond_t;
	bool isshutdown;

	



};
#endif