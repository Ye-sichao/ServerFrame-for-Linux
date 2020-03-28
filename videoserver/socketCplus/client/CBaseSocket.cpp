#include "CBaseSocket.h"

CBaseSocket::CBaseSocket( int domain,int type,int protocol )
{

	//˽�г�Ա���򵥵ĳ�ʼ��
	this->host_addr=NULL;
	//ִ��һ��socket������һ���׽���,����׽��ּȿ��Ը���������Ҳ�������ڿͻ���
	this->sock_fd=socket(domain,type,protocol);
	if(this->sock_fd==-1)
	{
		perror("socket fail:");
	}
}

CHostAddress CBaseSocket::GetAddr()
{
	return *(this->host_addr);
}

bool CBaseSocket::Bind( CHostAddress host_addr )
{
	int res;
	//���������õ���CHostSocket����ĸ��ֻ�ȡ��������ɰ��׽�������
	//�ȱ���һ�ݸ�˽������
	this->host_addr=&host_addr;
	//֮����а�
	res=bind(this->sock_fd,host_addr.GetAddr(),host_addr.GetLen());
	if(res==-1)
	{
		perror("bind fail:");
	}
	else 
	{
		cout<<"bind success"<<endl;
	}
	
}
int CBaseSocket::GetSocketFd()
{
	return this->sock_fd;
}

bool CBaseSocket::SetSocketFd( int fd )
{
	this->sock_fd=fd;
	return true;
}

bool CBaseSocket::CloseFd()
{
	int res;
	res=close(this->sock_fd);
	if(res==-1)
	{
		return false;
	}
	return true;
}