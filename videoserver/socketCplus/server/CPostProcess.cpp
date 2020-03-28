#include "CPostProcess.h"
#include "./sqlite3/qdata.h"
CPostProcess::CPostProcess()
{
	
		char buf[2048]="0";
		this->onlinelist=new map<int ,char*>;
		//初始化应答共享内存
		this->Ansshm=new CMemShBase(5);
		this->Ansshm->Shm_Create((key_t)520,10240);
		this->Ansshm->Shm_At();
		this->postsem=new CSemBase((key_t)1234,2);
			//根据共享内存的分块个数来决定信号量值的大小，先设置为5block
		//cout<<"post sem 0 init is "<<this->postsem->GetVal(0)<<endl;
		//cout<<"post sem 1 init is "<<this->postsem->GetVal(1)<<endl;
		//创建共享内存读的共享内存
		this->postshm=new CMemShBase(5);//内存块分为5块
		this->postshm->Shm_Create((key_t)1314,10240); //每块内存刚好20字节
		this->postshm->Shm_At();//映射到本进程
		//创建写入的共享内存
		this->WriteShm=new CMemShBase(5);
		this->WriteShm->Shm_Create((key_t)510,10240);
		this->WriteShm->Shm_At();
		cout<<"post process first read"<<endl;
		//创建写入的信号量
		this->sem=new CSemBase((key_t)1026,2);
		this->sem->SetSem(0,5);
		this->sem->SetSem(1,0);
		while(1)  //循环读取共享内存内的数据并且进行解析，不断执行任务
		{
			
			//不断进行对共享内存的读取，
			//这边的信号量，对本方的信号量做P操作，读完之后对前置的信号量做V操作
			this->postsem->SemaphoreP(1);
			this->postshm->Read(buf);//读取共享内存的一块数据，然后添加到线程池进行解析
			this->postsem->SemaphoreV(0);
			//先测试一下读到的第一手数据有没有问题
			DEALPACK dealpck;
			memset(&dealpck,0,sizeof(DEALPACK));
			memcpy(&dealpck,buf,sizeof(DEALPACK));
			cout<<"the first data is "<<dealpck.fd<<"  "<<dealpck.pack_type<<endl;
			//读取到数据之后就加入任务进行解析操作
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

	//登录包业务处理
	if(onlinelist->empty())
	{
		//数据库保存用户名和密码
		int user_id=0;
		char sql[255]="0";
		char **res;
		int row,col;
		sprintf(sql,"select *from tbl_user");
		cout<<sql;
		pthread_mutex_lock(&this->mutex);
		Qdata::getMydata()->gettable("select *from tbl_user",res,row,col);
		pthread_mutex_unlock(&this->mutex);
		user_id=1001+row;//加上当前所在的用户数，分配一个用户ID
		memset(sql,0,255);
		cout<<"data "<<row<<res[4]<<endl;
		sprintf(sql,"insert into tbl_user values(%d,%d,'%s','%s')",user_id,dealpck.fd,loginpack.username,loginpack.paswd);
		pthread_mutex_lock(&this->mutex);
		Qdata::getMydata()->gettable(sql,res,row,col);//记录插入数据库
		pthread_mutex_unlock(&this->mutex);
		//如果是空的，直接加入在线用户列表
		cout<<"user name lenth is "<<endl;
		this->onlinelist->insert(pair<int,char*>(dealpck.fd,loginpack.username));
		loginans.is_ok=true;
		cout<<"add into the online list"<<endl;
	}
	else
	{
		cout<<"add check"<<endl;
		//如果已经有在线的用户，就验证一下是否重名，以及验证后续的功能
		if(this->onlinelist->find(dealpck.fd)==this->onlinelist->end())
		{
			//这边是没有重名，继续加入
			this->onlinelist->insert(pair<int,char*>(dealpck.fd,loginpack.username));
			loginans.is_ok=true;
		}
		else
		{
				//重名就不作处理，返回登录失败应答包
				loginans.is_ok=false;
		}

	}
	cout<<"login deal ok "<<endl;
	//处理结果，成功既返回一个应答包，由参数返回出去来
	Packaging(&loginans,Anspack,LoginAns_Type,sizeof(loginans));

}
CDealTask::CDealTask( CSemBase *sem,CMemShBase *rdshm,CMemShBase *wrshm,char *buf,map<int,char*> *onlinelist)
:CTaskBase()
{
	//线程锁初始化
	pthread_mutex_init(&this->mutex,NULL);
	this->sem=sem;
	this->shm=rdshm;
	this->wrshm=wrshm;
	this->onlinelist=onlinelist;
	//拷贝从共享内存读取的数据
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
	//根据读取到的数据直接进行解析操作
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
		cout<<"Video Play ："<<endl;
			this->VideoPlay(dealpck,buf);
			MemPack(RsVPlay_Type,&mempck,dealpck.fd,buf,sizeof(PLAYRSP));
			cout<<"post the mem deal pack type is "<<dealpck.pack_type<<endl;
		break;

	}
	//处理完成之后，此时的共享内存中的数据信息就是应答包的类型
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
	//数据库操作相关信息
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
	//判断请求包请求的视屏类型
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
					memcpy(msg.VideoPath,*(res+6+i),64); //数组越界
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
			memcpy(msg.VideoPath,*(res+6+i),64); //数组越界
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
			memcpy(msg.VideoPath,*(res+6+i),64); //数组越界
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
			memcpy(msg.VideoPath,*(res+6+i),64); //数组越界
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
			memcpy(msg.VideoPath,*(res+6+i),64); //数组越界
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
	//视屏信息赋值完成后，打包成应答包
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
	//数据库中对该对应的影片信息的点播数加1；
	sprintf(sql,"select video_count from tbl_video where video_id=%d",repck.video_num);
	Qdata::getMydata()->gettable(sql,res,row,col);
	playcount=atoi(res[1])+1;
	memset(sql,0,255);
	sprintf(sql,"update tbl_video set count=%d where video_id=%d",playcount,repck.video_num);
	Qdata::getMydata()->gettable(sql,res,row,col);
	//插入一条影片记录表的信息
	memset(sql,0,255);
	sprintf(sql,"insert into tbl_vdrecord values('%s',%d,%d)",repck.username,repck.video_num,0);
	Qdata::getMydata()->gettable(sql,res,row,col);
	//返回应答包的数据
	//取出电影的名称和显示图片的路径
	memset(sql,0,255);
	sprintf(sql,"select video_name,picture1_path from tbl_video where video_id=%d",repck.video_num);
	Qdata::getMydata()->gettable(sql,res,row,col);
	
	memcpy(rspck.Mvname,res[2],64);
	memcpy(rspck.image_path,res[3],64);
	cout<<"the movie name:"<<rspck.Mvname<<endl;
	cout<<"the movie path :"<<rspck.image_path<<endl;
	//打包返回
	Packaging(&rspck,Anspack,RsVPlay_Type,sizeof(rspck));



}