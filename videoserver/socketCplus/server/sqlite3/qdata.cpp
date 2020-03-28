#include "qdata.h"
Qdata *Qdata::mydata=NULL;
Qdata::Qdata()
{
    int ret;
    ret=sqlite3_open("./sqlite3/mydb",&this->db);
    if(ret!=SQLITE_OK)
    {
        cout<<"open db fail"<<endl;
    }
	//初始化线程锁
	if(pthread_mutex_init(&this->mutex,NULL)==0)
	{
		cout<<"mutex init success:"<<endl;
	}
}
Qdata::~Qdata()
{
    sqlite3_close(this->db);
}

Qdata * Qdata::getMydata()
{
    if(Qdata::mydata==NULL)
    {
        Qdata::mydata=new Qdata;
        return Qdata::mydata;
    }
    return Qdata::mydata;

}

void Qdata::getdata_prepare_ranklist()
{

}

void Qdata::gettable(const char * sqlorder,char **&presult,int &row,int &col)
{
    int errormsg;
	pthread_mutex_lock(&this->mutex);
    errormsg=sqlite3_get_table(this->db,sqlorder,&presult,&row,&col,0);
	pthread_mutex_unlock(&this->mutex);
    if(errormsg!=NULL)
    {
        cout<<"get data fail"<<errormsg<<endl;
    }
}
