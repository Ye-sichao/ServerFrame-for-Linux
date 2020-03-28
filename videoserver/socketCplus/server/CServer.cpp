#include "CServer.h"

CServer::CServer( int maxevents )
:CTcpEpoll(maxevents)
{
	this->sem=new CSemBase((key_t)1234,2);
	this->sem->SetSem(0,5);//���ݹ����ڴ�ķֿ�����������ź���ֵ�Ĵ�С��������Ϊ5block
	this->sem->SetSem(1,0);
	cout<<"init sem 1 state is "<<this->sem->GetVal(1)<<endl;
	cout<<"init sem 0 state is "<<this->sem->GetVal(0)<<endl;
	//���������ڴ�
	this->shm=new CMemShBase(5);//�ڴ���Ϊ5��
	this->shm->Shm_Create((key_t)1314,10240); //ÿ���ڴ�պ�20�ֽ�
	this->shm->Shm_At();//ӳ�䵽������
	//��������Ϣ�Ĺ����ڴ�
	this->rdshm=new CMemShBase(5);
	this->rdshm->Shm_Create((key_t)510,10240);
	this->rdshm->Shm_At();
	//���ڽ��չ����ڴ���ź���
	this->rdsem=new CSemBase((key_t)1026,2);
	//����һ��������Ϣ���߳�
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
	//������ѵ�һ�ν��յ���buf����ճ�����������������̳߳�

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
	//�ȱ��������������������
	char dealbuf[2048]="0";
	char data[2048]="0";
	memcpy(dealbuf,buf,1024);
	memset(&head,0,HEADSIZE);
	memset(&tail,0,TAILSIZE);
	p=buf;
	//ѭ��ȡ�������ҿ���һ���������İ���С�����̳߳��д���
	while(state)
	{
		memcpy(&head,p,HEADSIZE);
		memcpy(&tail,p+HEADSIZE+head.Pack_size,TAILSIZE);
		if(head.head_remask==Head_value&&tail.tail_remask==Tail_value) //˵����һ�������İ��������̳߳�
		{
			memcpy(data,p,HEADSIZE+head.Pack_size+TAILSIZE);
			//�����̳߳�������ջ����������������ж�
			cout<<"ADD task TO "<<endl;
			CMytask *task=new CMytask(fd,data,this->GetSem(),this->GetMem());
			this->threadpoll.AddTask(task);
			p+=(HEADSIZE+head.Pack_size+TAILSIZE);//ƫ��һ���������ľ���֮�󣬼��������ж�
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
	//����һ��д�빲���ڴ���ʽ�Ľṹ��
	//������������ѽ��д�빲���ڴ棬�����ý��̲���
	Unpacking(this->buf,data,&type);  //���������Ϣ����ɾ��
	cout<<"the type is "<<type<<endl;
	DEALPACK mempck;
	memset(&mempck,0,sizeof(DEALPACK));
	switch(type)
	{
	case Login_Type:   //��¼����д����Ϣ
				MemPack(Login_Type,&mempck,this->fd,this->buf,sizeof(LOGINPCK));
		break;
	case Revpck_Type:  //������ϸ��Ϣ�����
				MemPack(Revpck_Type,&mempck,this->fd,this->buf,sizeof(REVPCK));
		break;
	case ReVPlay_Type:     //�����㲥�����
				MemPack(ReVPlay_Type,&mempck,this->fd,this->buf,sizeof(PLAYREQ));
		break;
	}
	cout<<"the 0 sem val is "<<this->GetSem()->GetVal(0)<<endl;
	if(this->GetSem()->GetVal(0)<=5)  //����V�����޷��ڴﵽָ��ֵ��ʱ���������������ж�
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
	//����һ�´�������ݵ���ȷ��ʹ�ñ���
	LOGINANS ans;
	HEAD head;
	TAIL tail;
	//�߳�ִ�еľ����������ڶ�ȡ�����ڴ沢�Ҵ����Ӧ�����ҷ��ͻ�Ӧ��
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
	//��ȡ������֮�󣬾Ͷ����ݽ��н���
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
	//��֤�����¼������ɾ��
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
	//������֮�󣬷���
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