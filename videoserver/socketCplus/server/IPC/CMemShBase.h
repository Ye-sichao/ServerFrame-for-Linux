#ifndef CMEMSHBASE_H_
#define CMEMSHBASE_H_
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
//共享内存类的设计，需要一个互斥量来保护指针
class CMemShBase
{
public:
	CMemShBase(int BlockCount);
	~CMemShBase();
	bool Shm_Create(key_t key,size_t size);
	bool Shm_At();
	bool Shm_Dt();
	int Write(char *buf,int lenth);
	void Read(char *buf);
	int GetblockSize();
	void DelShm();
;protected:
	int shm_id;
	int BlockCount;
	int Size;
	int offset;
private:
		char *phead;
		char *pcur;
		char *ptail;
	pthread_mutex_t mutex_t;//保护指针的互斥锁
	

};
#endif