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
	CSemBase(key_t key,int semnum);  //��Ҫ���ź�����Ŀ���ⲿ����,һ��ֻ��Ҫһ��
	~CSemBase();
	bool SemaphoreP(unsigned short idex);  //P��������ָ�����ź�������ֵ-1,ֻ���һ���ź�������
	bool SemaphoreV(unsigned short idex);   //V���� �� ��ָ�����ź�������ֵ+1
	bool SetSem(int npos,int Val);   //�����ź�������ֵ������ָ����Ӧ���±꣬Ĭ�϶Ե�һ���ź������г�ʼ��
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