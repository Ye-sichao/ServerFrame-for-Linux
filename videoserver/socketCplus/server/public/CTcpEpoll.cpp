#include "CTcpEpoll.h"

CTcpEpoll::CTcpEpoll( int maxevents )
:CEpollBase(maxevents)
{
	//CHostAddress addr("192.168.24.230",7777);
	this->Tcpserver=new CServerSocket(this->addr);
	this->AddEvent(Tcpserver->GetSocketFd());

}	

CTcpEpoll::~CTcpEpoll()
{

}

void CTcpEpoll::Ondata(int fd,char *buf)
{
	

}

void CTcpEpoll::Run(int ncount)
{
	int i=0,rdcount;
	char buf[1024]="0";
	int client_socket;
	//说明有某一些套接口产生了读事件
	//cout<<"ssdsaddasdsadsadxzczxc"<<endl;
	for(i=0;i<ncount;i++)
	{
		if(res_event[i].data.fd==Tcpserver->GetSocketFd())
		{
			//如果这个套接字产生读事件，说明有新的客户端链接到服务器
			//再继续加入这个套接字，因为设置的最大监听数是二十
			client_socket=Tcpserver->Accept();
			this->AddEvent(client_socket);
			cout<<"a new client connect"<<endl;
			

		}
		else
		{
			//这边就是普通的客户端
			rdcount=read(res_event[i].data.fd,buf,sizeof(buf));
			if(rdcount<=0)
			{
				cout<<"read zero count"<<endl;
				cout<<"client down"<<endl;
				//删除这个套接字
				this->DelEvent(res_event[i].data.fd);

			}
			cout<<"into the ondata"<<endl;
			this->Ondata(res_event[i].data.fd,buf);

		}
	}
}