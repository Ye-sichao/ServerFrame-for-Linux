#include "CClientSocket.h"
#include "CHostAddress.h"
#include <iostream>
#include <unistd.h>
#include <string.h>
#include "TrPack.h"
using namespace std;

int main(int argc,char *argv[])
{
	char name[25]="admin";
	char buf[2048]="0";
	int type;
	char data[2048]="0";
	//先初始化一个地址的对象
	cout<<"ip is "<<argv[1]<<endl;
	CHostAddress addr(argv[1]);
	
	CClientSocket client(addr);
	
	//链接时发送一个登录包
	//cin>>"input your username : ">>name;
	//cin>>name;
	/*LOGINPCK login;
	memcpy(login.username,name,sizeof(name));
	memcpy(login.paswd,"123456",strlen("123456"));
	
	Packaging(&login,buf,Login_Type,sizeof(REVPCK));
	//cout<<"start write buf"<<endl;
	cout<<"ssssss"<<endl;
	client.Write(buf,sizeof(buf));
	cout<<"write ok"<<endl;	
	cout<<sizeof(buf)<<endl;
	memset(buf,0,2048);
	client.Read(buf,2048);
	HEAD head;
	memset(&head,0,HEADSIZE);
	memcpy(&head,buf,HEADSIZE);
	cout<<head.Pack_type<<endl;
	Unpacking(buf,data,&type);
	cout<<type<<endl;
	if(type==LoginAns_Type)
	{
		LOGINANS loginans;
		memset(&loginans,0,sizeof(LOGINANS));
		memcpy(&loginans,data,sizeof(loginans));
		if(loginans.is_ok==true)
		{
			cout<<"login success"<<endl;
		}
		else
		{
			cout<<"login fail"<<endl;
		}
	}
	//测试发送一个动漫视屏信息请求包

	REVPCK pck;
	memset(&pck,0,sizeof(REVPCK));
	pck.CurType=AnimeType;
	memset(buf,0,2048);
	Packaging(&pck,buf,Revpck_Type,sizeof(REVPCK));
	client.Write(buf,sizeof(buf));
	memset(buf,0,sizeof(buf));
	client.Read(buf,2048);
	memset(&head,0,HEADSIZE);
	memcpy(&head,buf,HEADSIZE);
	cout<<"video head is "<<head.Pack_type<<endl;
	memset(data,0,2048);
	Unpacking(buf,data,&type);
	if(type==Rsvpck_Type)
	{
		cout<<"get video msg ok"<<endl;
	}*/
	//测试视屏点播业务
	PLAYREQ playpck;
	PLAYRSP resplay;
	char mvname[64]="0";
	char path[64]="0";
	memset(&playpck,0,sizeof(PLAYREQ));
	memset(&resplay,0,sizeof(PLAYRSP));
	memset(buf,0,2048);
	memcpy(playpck.username,"admin",strlen("admin"));
	playpck.video_num=1;
	Packaging(&playpck,buf,ReVPlay_Type,sizeof(PLAYREQ));
	client.Write(buf,2048);
	memset(buf,0,2048);
	sleep(1);
	client.Read(buf,2048);
	Unpacking(buf,data,&type);
	if(type==RsVPlay_Type)
	{
		memcpy(&resplay,data,sizeof(resplay));
		cout<<"the name is :"<<resplay.Mvname<<endl;
		cout<<"the path is:"<<resplay.image_path<<endl;
	}
	else
	{
		cout<<"error"<<endl;
	}
	
	while(1)
	{
		
		sleep(2);
	}

	return 0;
	

}