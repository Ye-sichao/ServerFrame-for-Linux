#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
//�������ڳ�ʼ��һ���̣߳������ṩ�ӿ�
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