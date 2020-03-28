#include "CServerSocket.h"

CServerSocket::CServerSocket(CHostAddress host_addr,int domain,int type,int protocol)
:CTcpSocket(domain,type,protocol)
{
	this->host_addr=&host_addr;
	cout<<"server construcs"<<endl;
	this->Max_Connect_count=5;
	//�������ִ�н�������BIND��LISTEN����������
	this->Bind(*(this->host_addr));
	this->Listen();
}

void CServerSocket::Listen()
{
	int res;
	res=listen(this->sock_fd,this->Max_Connect_count);
	if(res==-1)
	{
		perror("listen fail:");
	}
	else
	{
		cout<<"listen success"<<this->sock_fd<<endl;
	}

}

int CServerSocket::Accept()
{
	int res;
	//��ȡ�ͻ��˵ĵ�ַ��Ϣ
	cout<<"wait client"<<endl;
	while((res=accept(this->sock_fd,this->GetRemoteAddr().GetAddr(),this->GetRemoteAddr().GetLenPtr()))>0)
	{
		cout<<"recve client"<<endl;
		return res;
	}

	
}