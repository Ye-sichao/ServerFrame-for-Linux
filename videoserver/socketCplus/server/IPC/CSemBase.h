#ifndef CSEMBASE_H_
#define CSEMBASE_H_
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
using namespace std;
union semun 
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
	struct seminfo *__buf;

};
class CSemBase
{
public:
	CSemBase(key_t key,int semnum);  //需要的信号量数目从外部传入,一般只需要一个
	~CSemBase();
	bool SemaphoreP(unsigned short idex);  //P操作，对指定的信号量的数值-1,只针对一个信号量进行
	bool SemaphoreV(unsigned short idex);   //V操作 ， 对指定的信号量的数值+1
	bool SetSem(int npos,int Val);   //设置信号量的数值，可以指定对应的下标，默认对第一个信号量进行初始化
	bool DelSem();
	int GetVal(unsigned short idex);
protected:
	int sem_id;
	int pro_id;
	key_t key;
	int val;
	bool isblock;


};
#endif