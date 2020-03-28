#include "CMemShBase.h"

CMemShBase::CMemShBase( int BlockCount )
{
	this->BlockCount=BlockCount;
	this->shm_id=0;
	this->phead=NULL;
	this->pcur=NULL;
	this->ptail=NULL;
	//初始化互斥量
	int res;
	res=pthread_mutex_init(&this->mutex_t,NULL);
	if(res!=0)
	{
		perror("pthread init error:");
	}
}

CMemShBase::~CMemShBase()
{
	
}

bool CMemShBase::Shm_Create(key_t key,size_t size)
{
	this->Size=size;
	this->shm_id=shmget(key,size,0666|IPC_CREAT);
	if(this->shm_id==0 || this->shm_id==-1)
	{
		shmctl(this->shm_id,IPC_RMID,NULL);
		this->shm_id=this->shm_id=shmget(key,size,0666|IPC_CREAT);
		if(this->shm_id==-1)
		{
			perror("shmget error:");
			return false;
		}
		
	}
	return true;

}

bool CMemShBase::Shm_At()
{
	this->phead=(char *)shmat(this->shm_id,NULL,8);
	if(this->phead==NULL)
	{
		perror("shmat seeror:");
	}
	this->pcur=this->phead;
	this->ptail=this->phead+this->Size;
	this->offset=this->Size/this->BlockCount;  //取整数，多出的字节数丢弃

	

}

bool CMemShBase::Shm_Dt()
{
	int res;
	res=shmdt(this->phead);
	if(res==-1)
	{
		perror("shmdt error:");
	}
}

int CMemShBase::Write( char *buf ,int lenth)
{
	printf("mem write success\n");
	if(this->pcur!=this->ptail)
	{
		//一个解包后的结构体大小写入
		pthread_mutex_lock(&this->mutex_t);
		memcpy(this->pcur,buf,lenth);
		pthread_mutex_unlock(&this->mutex_t);
		//写完之后指针偏移到下一块内存
		pthread_mutex_lock(&this->mutex_t);
		this->pcur+=this->offset;
		pthread_mutex_unlock(&this->mutex_t);
	}
	else
	{
		//如果到达尾部，就再偏移回头
		pthread_mutex_lock(&this->mutex_t);
		this->pcur=this->phead;
		pthread_mutex_unlock(&this->mutex_t);
		//一个解包后的结构体大小写入
		pthread_mutex_lock(&this->mutex_t);
		memcpy(this->pcur,buf,lenth);
		pthread_mutex_unlock(&this->mutex_t);
		//写完之后指针偏移到下一块内存
		pthread_mutex_lock(&this->mutex_t);
		this->pcur+=this->offset;
		pthread_mutex_unlock(&this->mutex_t);
	}
}

void CMemShBase::Read(char *buf)
{
	int nbits;
	if(this->pcur!=this->ptail)
	{
		pthread_mutex_lock(&this->mutex_t);
		memcpy(buf,this->pcur,offset);//读取一块共享内存块的大小
		memset(this->pcur,0,offset);
		pthread_mutex_unlock(&this->mutex_t);

		pthread_mutex_lock(&this->mutex_t);
		this->pcur+=this->offset;
		pthread_mutex_unlock(&this->mutex_t);
	}
	else
	{
		pthread_mutex_lock(&this->mutex_t);
		this->pcur=this->phead;
		pthread_mutex_unlock(&this->mutex_t);
		pthread_mutex_lock(&this->mutex_t);
		memcpy(buf,this->pcur,offset);//读取一块共享内存块的大小
		memset(this->pcur,0,offset);
		pthread_mutex_unlock(&this->mutex_t);
		pthread_mutex_lock(&this->mutex_t);
		this->pcur+=this->offset;
		pthread_mutex_unlock(&this->mutex_t);
	}
	

	
}

int CMemShBase::GetblockSize()
{
	return this->offset;
}

void CMemShBase::DelShm()
{
	if(shmctl(this->shm_id,IPC_RMID,NULL)==-1)
	{
		perror("delete shm error:");
	}
}