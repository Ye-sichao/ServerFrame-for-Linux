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
	//˵����ĳһЩ�׽ӿڲ����˶��¼�
	//cout<<"ssdsaddasdsadsadxzczxc"<<endl;
	for(i=0;i<ncount;i++)
	{
		if(res_event[i].data.fd==Tcpserver->GetSocketFd())
		{
			//�������׽��ֲ������¼���˵�����µĿͻ������ӵ�������
			//�ټ�����������׽��֣���Ϊ���õ����������Ƕ�ʮ
			client_socket=Tcpserver->Accept();
			this->AddEvent(client_socket);
			cout<<"a new client connect"<<endl;
			

		}
		else
		{
			//��߾�����ͨ�Ŀͻ���
			rdcount=read(res_event[i].data.fd,buf,sizeof(buf));
			if(rdcount<=0)
			{
				cout<<"read zero count"<<endl;
				cout<<"client down"<<endl;
				//ɾ������׽���
				this->DelEvent(res_event[i].data.fd);

			}
			cout<<"into the ondata"<<endl;
			this->Ondata(res_event[i].data.fd,buf);

		}
	}
}