#include "CTcpSocket.h"

CTcpSocket::CTcpSocket()
{

}

CTcpSocket::CTcpSocket( int domain,int type,int protocol )
:CBaseSocket(domain,type,protocol)  //直接调用父类的方法生成一个Socket套接字
{
	//对私有属性做初始化
	this->pro_addr=NULL;
	this->remote_addr=NULL;
	this->pro_addr=this->host_addr;
}

CTcpSocket::CTcpSocket( CHostAddress host_addr,int domain,int type,int protocol/*=0*/ )
:CBaseSocket(domain,type,protocol)  //直接调用父类的方法生成一个Socket套接字
{
	//对私有属性做初始化
	this->pro_addr=NULL;
	this->remote_addr=NULL;
	this->pro_addr=this->host_addr;
}
CHostAddress CTcpSocket::GetRemoteAddr()
{
	return (this->remote_addr);
}

void CTcpSocket::SetRemoteAddr( CHostAddress remote_addr )
{
	//拷贝
	this->remote_addr=remote_addr;
}

void CTcpSocket::Connect( CHostAddress host_addr ) //方法留给客户端的类来调用
{
	int res;
	res=connect(this->sock_fd,host_addr.GetAddr(),host_addr.GetLen());
	if(res!=0)
	{
		perror("connect fail\n");
	}
}

void CTcpSocket::Read( char *buf,int lenth )
{
		read(this->sock_fd,buf,lenth);
	
}

void CTcpSocket::Write( char *buf,int lenth )
{
	int n=0;
		n=write(this->sock_fd,buf,lenth);
		cout<<"Write "<<n<<"bite"<<endl;

}