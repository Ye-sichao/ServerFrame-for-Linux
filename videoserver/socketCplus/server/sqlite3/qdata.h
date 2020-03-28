#ifndef QDATA_H
#define QDATA_H
#include "sqlite3.h"
#include <pthread.h>
#include <iostream>
using namespace std;
class Qdata
{
public:
    static Qdata * getMydata();
    ~Qdata();
    void getdata_prepare_ranklist();
    void gettable(const char * sqlorder,char **&presult,int &row,int &col);


private:
    Qdata();
    sqlite3_stmt *state;
    sqlite3 *db;
    static Qdata * mydata;
	//�����ݿ���ϻ�����
	pthread_mutex_t mutex;

};

#endif // QDATA_H
