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
//��������ڽ��հ����Ҵ�����Ӧ�������������ά��һ�������б�
class CDealTask:public CTaskBase
{
public:
	CDealTask(CSemBase *sem,CMemShBase *rdshm,CMemShBase *wrshm,char *buf,map<int,char*> *onlinelist);
	~CDealTask();
	void Run(); //��������̣����������ɲ���֮�󷵻�Ӧ���
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
	//�߳���
	pthread_mutex_t mutex;
};

class CPostProcess
{
public:
	CPostProcess();
	~CPostProcess();
	//�԰��Ĵ��������ȴ����¼��
	
private:
	CThreadPoll pthreadpoll; //�̳߳����ڴ�������
	CSemBase *postsem;  //�ź���ִ��V���� 
	CMemShBase *postshm; //�ӹ����ڴ��ȡ�����Ľ��
	CMemShBase *Ansshm;   //���ظ�ǰ�ý���Ӧ����Ĺ����ڴ�
	map<int,char*> *onlinelist; //ά��һ�������û��б�
	CMemShBase *WriteShm;//���ڴ���Ӧ�����ǰ�ý��̵Ĺ����ڴ�
	CSemBase *sem;
	
	
};
#endif