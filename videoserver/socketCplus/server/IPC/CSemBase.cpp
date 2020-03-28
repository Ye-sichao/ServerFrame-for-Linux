
#include "CSemBase.h"
CSemBase::CSemBase( key_t key,int semnum)
{
	this->sem_id=semget(key,semnum,0666|IPC_CREAT);
	if(this->sem_id==-1)
	{
		perror("sema create error:");
	}
}

CSemBase::~CSemBase()
{
	
}

bool CSemBase::SetSem( int npos,int Val )
{
	this->val=Val;
	union semun arg;
	arg.val=Val;
	if(semctl(this->sem_id,npos,SETVAL,arg)<0)
	{
		perror("sem set error:");
	}
}

bool CSemBase::DelSem()
{
	if(semctl(this->sem_id,0,IPC_RMID)<0)
	{
		perror("sem del error:");
	}
}

bool CSemBase::SemaphoreP( unsigned short idex )
{
	struct sembuf buf={idex,-1,};
	if(semop(this->sem_id,&buf,1)<0)
	{
		perror("p operatetor error:");
	}
}

bool CSemBase::SemaphoreV( unsigned short idex )
{
	cout<<semctl(this->sem_id,idex,GETVAL)<<endl;
	struct sembuf buf={idex,1,SEM_UNDO};
	if(semop(this->sem_id,&buf,1)<0)
	{
		perror("v operatetor error:");
	}
}

int CSemBase::GetVal(unsigned short idex)
{
	return semctl(this->sem_id,idex,GETVAL);
}
