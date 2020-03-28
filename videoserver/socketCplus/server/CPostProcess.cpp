#include "CPostProcess.h"
#include "./sqlite3/qdata.h"
CPostProcess::CPostProcess()
{
	
		char buf[2048]="0";
		this->onlinelist=new map<int ,char*>;
		//��ʼ��Ӧ�����ڴ�
		this->Ansshm=new CMemShBase(5);
		this->Ansshm->Shm_Create((key_t)520,10240);
		this->Ansshm->Shm_At();
		this->postsem=new CSemBase((key_t)1234,2);
			//���ݹ����ڴ�ķֿ�����������ź���ֵ�Ĵ�С��������Ϊ5block
		//cout<<"post sem 0 init is "<<this->postsem->GetVal(0)<<endl;
		//cout<<"post sem 1 init is "<<this->postsem->GetVal(1)<<endl;
		//���������ڴ���Ĺ����ڴ�
		this->postshm=new CMemShBase(5);//�ڴ���Ϊ5��
		this->postshm->Shm_Create((key_t)1314,10240); //ÿ���ڴ�պ�20�ֽ�
		this->postshm->Shm_At();//ӳ�䵽������
		//����д��Ĺ����ڴ�
		this->WriteShm=new CMemShBase(5);
		this->WriteShm->Shm_Create((key_t)510,10240);
		this->WriteShm->Shm_At();
		cout<<"post process first read"<<endl;
		//����д����ź���
		this->sem=new CSemBase((key_t)1026,2);
		this->sem->SetSem(0,5);
		this->sem->SetSem(1,0);
		while(1)  //ѭ����ȡ�����ڴ��ڵ����ݲ��ҽ��н���������ִ������
		{
			
			//���Ͻ��жԹ����ڴ�Ķ�ȡ��
			//��ߵ��ź������Ա������ź�����P����������֮���ǰ�õ��ź�����V����
			this->postsem->SemaphoreP(1);
			this->postshm->Read(buf);//��ȡ�����ڴ��һ�����ݣ�Ȼ����ӵ��̳߳ؽ��н���
			this->postsem->SemaphoreV(0);
			//�Ȳ���һ�¶����ĵ�һ��������û������
			DEALPACK dealpck;
			memset(&dealpck,0,sizeof(DEALPACK));
			memcpy(&dealpck,buf,sizeof(DEALPACK));
			cout<<"the first data is "<<dealpck.fd<<"  "<<dealpck.pack_type<<endl;
			//��ȡ������֮��ͼ���������н�������
			cout<<"post P read buf will into analyse "<<endl;
			CDealTask *mytask=new CDealTask(this->sem,this->postshm,this->WriteShm,buf,this->onlinelist);
			this->pthreadpoll.AddTask(mytask);

			
		}
}

CPostProcess::~CPostProcess()
{

}

void CDealTask::LoginDeal( DEALPACK dealpck ,char *Anspack)
{
	
	LOGINANS loginans;
	LOGINPCK loginpack;
	memset(&loginans,0,sizeof(LOGINANS));
	memset(&loginpack,0,sizeof(LOGINPCK));
	memcpy(&loginpack,dealpck.pack_msg,sizeof(LOGINPCK));
	cout<<"into the login deal "<<sizeof(dealpck.pack_msg)<<"  "<<sizeof(LOGINPCK)<<endl;

	//��¼��ҵ����
	if(onlinelist->empty())
	{
		//���ݿⱣ���û���������
		int user_id=0;
		char sql[255]="0";
		char **res;
		int row,col;
		sprintf(sql,"select *from tbl_user");
		cout<<sql;
		pthread_mutex_lock(&this->mutex);
		Qdata::getMydata()->gettable("select *from tbl_user",res,row,col);
		pthread_mutex_unlock(&this->mutex);
		user_id=1001+row;//���ϵ�ǰ���ڵ��û���������һ���û�ID
		memset(sql,0,255);
		cout<<"data "<<row<<res[4]<<endl;
		sprintf(sql,"insert into tbl_user values(%d,%d,'%s','%s')",user_id,dealpck.fd,loginpack.username,loginpack.paswd);
		pthread_mutex_lock(&this->mutex);
		Qdata::getMydata()->gettable(sql,res,row,col);//��¼�������ݿ�
		pthread_mutex_unlock(&this->mutex);
		//����ǿյģ�ֱ�Ӽ��������û��б�
		cout<<"user name lenth is "<<endl;
		this->onlinelist->insert(pair<int,char*>(dealpck.fd,loginpack.username));
		loginans.is_ok=true;
		cout<<"add into the online list"<<endl;
	}
	else
	{
		cout<<"add check"<<endl;
		//����Ѿ������ߵ��û�������֤һ���Ƿ��������Լ���֤�����Ĺ���
		if(this->onlinelist->find(dealpck.fd)==this->onlinelist->end())
		{
			//�����û����������������
			this->onlinelist->insert(pair<int,char*>(dealpck.fd,loginpack.username));
			loginans.is_ok=true;
		}
		else
		{
				//�����Ͳ����������ص�¼ʧ��Ӧ���
				loginans.is_ok=false;
		}

	}
	cout<<"login deal ok "<<endl;
	//���������ɹ��ȷ���һ��Ӧ������ɲ������س�ȥ��
	Packaging(&loginans,Anspack,LoginAns_Type,sizeof(loginans));

}
CDealTask::CDealTask( CSemBase *sem,CMemShBase *rdshm,CMemShBase *wrshm,char *buf,map<int,char*> *onlinelist)
:CTaskBase()
{
	//�߳�����ʼ��
	pthread_mutex_init(&this->mutex,NULL);
	this->sem=sem;
	this->shm=rdshm;
	this->wrshm=wrshm;
	this->onlinelist=onlinelist;
	//�����ӹ����ڴ��ȡ������
	memcpy(this->buf,buf,1024);
	this->tty_fd=open("/opt/poststart.txt",0666|O_RDWR|O_CREAT);
}

CDealTask::~CDealTask()
{

}

void CDealTask::Run()
{
	char buf[2048]="0";
	DEALPACK dealpck;
	DEALPACK mempck;
	memset(&dealpck,0,DEALPCSIZE);
	//���ݶ�ȡ��������ֱ�ӽ��н�������
	memcpy(&dealpck,this->buf,DEALPCSIZE);
	cout<<"step into mem pack deal "<<dealpck.pack_type<<endl;
	switch(dealpck.pack_type)
	{
	case Login_Type:
		this->LoginDeal(dealpck,buf);
		MemPack(LoginAns_Type,&mempck,dealpck.fd,buf,sizeof(LOGINANS));	
		cout<<"post the mem deal pack type is "<<dealpck.pack_type<<endl;
		break;
	case Revpck_Type:
			this->VideoRecv(dealpck,buf);
			MemPack(Rsvpck_Type,&mempck,dealpck.fd,buf,sizeof(RSVPCK));
			cout<<"post the mem deal pack type is "<<dealpck.pack_type<<endl;
		break;
	case ReVPlay_Type:
		cout<<"Video Play ��"<<endl;
			this->VideoPlay(dealpck,buf);
			MemPack(RsVPlay_Type,&mempck,dealpck.fd,buf,sizeof(PLAYRSP));
			cout<<"post the mem deal pack type is "<<dealpck.pack_type<<endl;
		break;

	}
	//�������֮�󣬴�ʱ�Ĺ����ڴ��е�������Ϣ����Ӧ���������
	cout<<"post write mem for server and this val is "<<this->sem->GetVal(1)<<endl;
	if(this->sem->GetVal(1)<5)
	{
		
		this->sem->SemaphoreP(0);
		cout<<"type= "<<mempck.pack_type<<endl;
		this->wrshm->Write((char *)&mempck,sizeof(mempck));	
		this->sem->SemaphoreV(1);
		cout<<"post pack len is "<<mempck.pck_Lenth<<" "<<sizeof(mempck)<<endl;
		cout<<"post for server write ok the data is "<<mempck.fd<<"type "<<mempck.pack_type<<endl;
	}	
}

void CDealTask::VideoRecv( DEALPACK dealpck,char *Anspack )
{
	//���ݿ���������Ϣ
	int i=0;
	char **res;
	int row,col;
	REVPCK rppck;
	RSVPCK rspck;
	VIDEOMSG msg;
	memset(&rppck,0,sizeof(REVPCK));
	memset(&rspck,0,sizeof(RSVPCK));
	memset(&msg,0,sizeof(VIDEOMSG));
	memcpy(&rppck,dealpck.pack_msg,sizeof(REVPCK));
	//�ж�������������������
	switch(rppck.CurType)
	{
	case AnimeType:
		Qdata::getMydata()->gettable("select picture1_path,picture2_path,picture3_path,video_name,video_path,video_count from tbl_video as a,tbl_anime as b where a.video_id=b.video_id",res,row,col);
		cout<<"getdata "<<row<<endl;
		/*cout<<res[5]<<endl;
		cout<<res[6]<<endl;
		cout<<res[7]<<endl;*/
			rspck.video_count=row;
				while(row!=0)
				{
					memcpy(msg.imagePath1,*(res+6+i),64);
					i++;
					memcpy(msg.imagePath2,*(res+6+i),64);
					i++;
					memcpy(msg.MvName,*(res+6+i),64);
					i++;
					memcpy(msg.VideoPath,*(res+6+i),64); //����Խ��
					i++;
					msg.cout=(int)*(*(res+6+i));
					i++;
					i-=5;
					memcpy(&(rspck.videomsg[i]),&msg,sizeof(VIDEOMSG));
					i+=5;
					row--;
					memset(&msg,0,sizeof(VIDEOMSG));

				}
		break;
	case MovieType :
		Qdata::getMydata()->gettable("select picture1_path,picture2_path,picture3_path,video_name,video_path,video_count from tbl_video as a,tbl_anime as b where a.video_id=b.video_id",res,row,col);
		cout<<"getdata "<<row<<endl;
		/*cout<<res[5]<<endl;
		cout<<res[6]<<endl;
		cout<<res[7]<<endl;*/
		rspck.video_count=row;
		while(row!=0)
		{
			memcpy(msg.imagePath1,*(res+6+i),64);
			i++;
			memcpy(msg.imagePath2,*(res+6+i),64);
			i++;
			memcpy(msg.MvName,*(res+6+i),64);
			i++;
			memcpy(msg.VideoPath,*(res+6+i),64); //����Խ��
			i++;
			msg.cout=(int)*(*(res+6+i));
			i++;
			i-=5;
			memcpy(&(rspck.videomsg[i]),&msg,sizeof(VIDEOMSG));
			i+=5;
			row--;
			memset(&msg,0,sizeof(VIDEOMSG));

		}

		break;
	case MusicType :
		Qdata::getMydata()->gettable("select picture1_path,picture2_path,picture3_path,video_name,video_path,video_count from tbl_video as a,tbl_music as b where a.video_id=b.video_id",res,row,col);
		cout<<"getdata "<<row<<endl;
		/*cout<<res[5]<<endl;
		cout<<res[6]<<endl;
		cout<<res[7]<<endl;*/
		rspck.video_count=row;
		while(row!=0)
		{
			memcpy(msg.imagePath1,*(res+6+i),64);
			i++;
			memcpy(msg.imagePath2,*(res+6+i),64);
			i++;
			memcpy(msg.MvName,*(res+6+i),64);
			i++;
			memcpy(msg.VideoPath,*(res+6+i),64); //����Խ��
			i++;
			msg.cout=(int)*(*(res+6+i));
			i++;
			i-=5;
			memcpy(&(rspck.videomsg[i]),&msg,sizeof(VIDEOMSG));
			i+=5;
			row--;
			memset(&msg,0,sizeof(VIDEOMSG));

		}
		break;
	case VarietyType :
		Qdata::getMydata()->gettable("select picture1_path,picture2_path,picture3_path,video_name,video_path,video_count from tbl_video as a,tbl_variety as b where a.video_id=b.video_id",res,row,col);
		cout<<"getdata "<<row<<endl;
		/*cout<<res[5]<<endl;
		cout<<res[6]<<endl;
		cout<<res[7]<<endl;*/
		rspck.video_count=row;
		while(row!=0)
		{
			memcpy(msg.imagePath1,*(res+6+i),64);
			i++;
			memcpy(msg.imagePath2,*(res+6+i),64);
			i++;
			memcpy(msg.MvName,*(res+6+i),64);
			i++;
			memcpy(msg.VideoPath,*(res+6+i),64); //����Խ��
			i++;
			msg.cout=(int)*(*(res+6+i));
			i++;
			i-=5;
			memcpy(&(rspck.videomsg[i]),&msg,sizeof(VIDEOMSG));
			i+=5;
			row--;
			memset(&msg,0,sizeof(VIDEOMSG));

		}
		break;
	case GameType:
		Qdata::getMydata()->gettable("select picture1_path,picture2_path,picture3_path,video_name,video_path,video_count from tbl_video as a,tbl_game as b where a.video_id=b.video_id",res,row,col);
		cout<<"getdata "<<row<<endl;
		/*cout<<res[5]<<endl;
		cout<<res[6]<<endl;
		cout<<res[7]<<endl;*/
		rspck.video_count=row;
		while(row!=0)
		{
			memcpy(msg.imagePath1,*(res+6+i),64);
			i++;
			memcpy(msg.imagePath2,*(res+6+i),64);
			i++;
			memcpy(msg.MvName,*(res+6+i),64);
			i++;
			memcpy(msg.VideoPath,*(res+6+i),64); //����Խ��
			i++;
			msg.cout=(int)*(*(res+6+i));
			i++;
			i-=5;
			memcpy(&(rspck.videomsg[i]),&msg,sizeof(VIDEOMSG));
			i+=5;
			row--;
			memset(&msg,0,sizeof(VIDEOMSG));

		}
		break;
	}
	//������Ϣ��ֵ��ɺ󣬴����Ӧ���
	Packaging(&rspck,Anspack,Rsvpck_Type,sizeof(rspck));
}

void CDealTask::VideoPlay( DEALPACK dealpck,char *Anspack )
{
	char **res;
	int col,row,playcount;
	char sql[255]="0";
	PLAYREQ repck; 
	PLAYRSP rspck;
	memset(&repck,0,sizeof(PLAYREQ));
	memset(&rspck,0,sizeof(PLAYRSP));
	memcpy(&repck,dealpck.pack_msg,dealpck.pck_Lenth);
	//���ݿ��жԸö�Ӧ��ӰƬ��Ϣ�ĵ㲥����1��
	sprintf(sql,"select video_count from tbl_video where video_id=%d",repck.video_num);
	Qdata::getMydata()->gettable(sql,res,row,col);
	playcount=atoi(res[1])+1;
	memset(sql,0,255);
	sprintf(sql,"update tbl_video set count=%d where video_id=%d",playcount,repck.video_num);
	Qdata::getMydata()->gettable(sql,res,row,col);
	//����һ��ӰƬ��¼�����Ϣ
	memset(sql,0,255);
	sprintf(sql,"insert into tbl_vdrecord values('%s',%d,%d)",repck.username,repck.video_num,0);
	Qdata::getMydata()->gettable(sql,res,row,col);
	//����Ӧ���������
	//ȡ����Ӱ�����ƺ���ʾͼƬ��·��
	memset(sql,0,255);
	sprintf(sql,"select video_name,picture1_path from tbl_video where video_id=%d",repck.video_num);
	Qdata::getMydata()->gettable(sql,res,row,col);
	
	memcpy(rspck.Mvname,res[2],64);
	memcpy(rspck.image_path,res[3],64);
	cout<<"the movie name:"<<rspck.Mvname<<endl;
	cout<<"the movie path :"<<rspck.image_path<<endl;
	//�������
	Packaging(&rspck,Anspack,RsVPlay_Type,sizeof(rspck));



}