#include "CServer.h"

CServer::CServer( int maxevents )
:CTcpEpoll(maxevents)
{
	this->sem=new CSemBase((key_t)1234,2);
	this->sem->SetSem(0,5);//根据共享内存的分块个数来决定信号量值的大小，先设置为5block
	this->sem->SetSem(1,0);
	cout<<"init sem 1 state is "<<this->sem->GetVal(1)<<endl;
	cout<<"init sem 0 state is "<<this->sem->GetVal(0)<<endl;
	//创建共享内存
	this->shm=new CMemShBase(5);//内存块分为5块
	this->shm->Shm_Create((key_t)1314,10240); //每块内存刚好20字节
	this->shm->Shm_At();//映射到本进程
	//创建读消息的共享内存
	this->rdshm=new CMemShBase(5);
	this->rdshm->Shm_Create((key_t)510,10240);
	this->rdshm->Shm_At();
	//用于接收共享内存的信号量
	this->rdsem=new CSemBase((key_t)1026,2);
	//创建一个发送消息的线程
	this->sendthread=new CSendThread(this->rdsem,this->rdshm);
	

}

CServer::~CServer()
{
	//delete this->sem;
	/*this->shm->Shm_Dt();
	this->rdshm->Shm_Dt();
	this->shm->DelShm();
	this->rdshm->DelShm();*/
}

void CServer::Ondata( int fd,char *buf )
{
	//在这里把第一次接收到的buf进行粘包处理完后，任务加入线程池

	/*cout<<"ADD task TO "<<endl;
	CMytask *task=new CMytask(fd,buf,this->GetSem(),this->GetMem());
	this->threadpoll.AddTask(task);*/
	this->StickPckDeal(buf,fd);
	

}

CSemBase * CServer::GetSem()
{
	return this->sem;
}

CMemShBase * CServer::GetMem()
{
	return this->shm;
}

void CServer::StickPckDeal( char *buf ,int fd)
{
	HEAD head;
	TAIL tail;
	bool state=true;
	char *p=NULL;
	//先保存下整个缓冲包的内容
	char dealbuf[2048]="0";
	char data[2048]="0";
	memcpy(dealbuf,buf,1024);
	memset(&head,0,HEADSIZE);
	memset(&tail,0,TAILSIZE);
	p=buf;
	//循环取包，并且拷贝一定的完整的包大小放入线程池中处理
	while(state)
	{
		memcpy(&head,p,HEADSIZE);
		memcpy(&tail,p+HEADSIZE+head.Pack_size,TAILSIZE);
		if(head.head_remask==Head_value&&tail.tail_remask==Tail_value) //说明是一个完整的包，丢入线程池
		{
			memcpy(data,p,HEADSIZE+head.Pack_size+TAILSIZE);
			//加入线程池任务，清空缓冲区，继续往下判断
			cout<<"ADD task TO "<<endl;
			CMytask *task=new CMytask(fd,data,this->GetSem(),this->GetMem());
			this->threadpoll.AddTask(task);
			p+=(HEADSIZE+head.Pack_size+TAILSIZE);//偏移一个完整包的距离之后，继续往下判断
		}
		else
		{
			state=false;
		}
	}
	
}
CMytask::CMytask(int fd,char *buf,CSemBase *sem,CMemShBase * shm)
:CTaskBase(fd,buf)
{
	this->sem=sem;
	this->shm=shm;
}

CMytask::~CMytask()
{
	
}

void CMytask::Run()
{
	cout<<"step into pack process"<<endl;
	char data[1024]="0";
	int type;
	//定义一个写入共享内存形式的结构体
	//做解包操作，把结果写入共享内存，供后置进程操作
	Unpacking(this->buf,data,&type);  //解包测试信息，可删除
	cout<<"the type is "<<type<<endl;
	DEALPACK mempck;
	memset(&mempck,0,sizeof(DEALPACK));
	switch(type)
	{
	case Login_Type:   //登录包的写入信息
				MemPack(Login_Type,&mempck,this->fd,this->buf,sizeof(LOGINPCK));
		break;
	case Revpck_Type:  //视屏详细信息请求包
				MemPack(Revpck_Type,&mempck,this->fd,this->buf,sizeof(REVPCK));
		break;
	case ReVPlay_Type:     //视屏点播请求包
				MemPack(ReVPlay_Type,&mempck,this->fd,this->buf,sizeof(PLAYREQ));
		break;
	}
	cout<<"the 0 sem val is "<<this->GetSem()->GetVal(0)<<endl;
	if(this->GetSem()->GetVal(0)<=5)  //由于V操作无法在达到指定值的时候阻塞，所以先判断
	{
		
		this->GetSem()->SemaphoreP(0); 
		cout<<"the p after the val is"<<this->GetSem()->GetVal(0)<<endl;
		cout<<"the write mempack type is "<<mempck.pack_type<<"the pack fd "<<mempck.fd<<endl;
		this->GetMem()->Write((char *)&mempck,sizeof(mempck));
		cout<<"server write shm ok"<<endl;
		this->GetSem()->SemaphoreV(1);
	}
	
	

}

CSemBase * CMytask::GetSem()
{
	return this->sem;
}

CMemShBase * CMytask::GetMem()
{
	return this->shm;
}

CSendThread::CSendThread( CSemBase *sem,CMemShBase *rdshm )
:CPthreadBase()
{
	this->sem=sem;
	this->shm=rdshm;
}

CSendThread::~CSendThread()
{

}

void CSendThread::run()
{
	//测试一下打包的数据的正确性使用变量
	LOGINANS ans;
	HEAD head;
	TAIL tail;
	//线程执行的具体任务，用于读取共享内存并且打包回应包并且发送回应包
	cout<<"i m the send thread"<<endl;
	int fd;
	char data[2048]="0";
	char buf[2048]="0";
	DEALPACK mempck;
	memset(&mempck,0,sizeof(DEALPACK));
	cout<<"will P "<<endl;
	cout<<"send fun p the val is = "<<this->sem->GetVal(1)<<endl;
	this->sem->SemaphoreP(1);
	this->shm->Read(buf);
	this->sem->SemaphoreV(0);
	cout<<"the buf size is "<<sizeof(buf)<<endl;
	//读取到数据之后，就对数据进行解析
	memcpy(&mempck,buf,sizeof(DEALPACK));
	cout<<"the mempack will send size is "<<sizeof(mempck)<<"data is "<<mempck.fd<<mempck.pack_type<<endl;
	fd=mempck.fd;
	/*switch(mempck.pack_type)
	{
	case Login_Type:

		break;
	}*/
	cout<<"the fd is "<<fd<<"the pack type is "<<mempck.pack_type<<"the pack lenth is "<<mempck.pck_Lenth<<endl;
	Packaging(mempck.pack_msg,data,mempck.pack_type,mempck.pck_Lenth);
	//验证请求登录包，可删除
	if(mempck.pack_type==RsVPlay_Type)
	{
		char mydata[1024]="0";
		int mytype;
		PLAYRSP res;
		Unpacking(data,mydata,&mytype);
		memset(&res,0,sizeof(PLAYRSP));
		memcpy(&res,mydata,sizeof(PLAYRSP));
		
		cout<<"will send name is :"<<res.Mvname<<endl;	
	}
	//打包完成之后，发包
	memset(&ans,0,sizeof(LOGINANS));
	memset(&head,0,sizeof(HEAD));
	memset(&tail,0,sizeof(TAIL));
	memcpy(&ans,data+HEADSIZE,sizeof(LOGINANS));
	memcpy(&head,data,sizeof(HEAD));
	memcpy(&tail,data+HEADSIZE+mempck.pck_Lenth,TAILSIZE);
	cout<<"the head mask is "<<head.head_remask<<"the tail mask "<<tail.tail_remask<<"loginans len is "<<sizeof(LOGINANS)<<endl;
	cout<<"the head type is "<<head.Pack_type<<"the buf size is "<<head.Pack_size<<endl;
	write(fd,data,sizeof(data));
	cout<<"send ok"<<endl;


}