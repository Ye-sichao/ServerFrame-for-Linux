#include "CBaseSocket.h"

CBaseSocket::CBaseSocket( int domain,int type,int protocol )
{

	//私有成员作简单的初始化
	this->host_addr=NULL;
	//执行一次socket，产生一个套接字,这个套接字既可以给服务器端也可以用于客户端
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
	//这个步骤就用到了CHostSocket里面的各种获取函数来完成绑定套接字流程
	//先保存一份给私有属性
	this->host_addr=&host_addr;
	//之后进行绑定
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