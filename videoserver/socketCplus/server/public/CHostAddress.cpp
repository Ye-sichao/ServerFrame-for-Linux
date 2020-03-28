#include "CHostAddress.h"
CHostAddress::CHostAddress( const char *ip,unsigned short port )
{
	//私有属性保存IP和端口号，并设置结构体的值,清空结构体的值
	if(ip!=NULL)
	{
		memcpy(this->ip,ip,32);
	}	
	
	if(port<=1024)
	{
		this->port=(unsigned short)7777;
	}
	this->port=port;
	memset(&(this->addr),0,sizeof(this->addr));
	
	//设置结构体的各项元素
	this->addr.sin_family=PF_INET;//使用IPV4
	this->addr.sin_port=htons(this->port);//本机转网络端口号
	if(ip==NULL)
	{
		this->addr.sin_addr.s_addr=INADDR_ANY;
	}
	else
	{
		cout<<"ip save ok "<<this->ip<<endl;
		this->addr.sin_addr.s_addr=inet_addr(this->ip);//默认是客户端的IP，所以需要转换，如果是服务器端，就是自动寻找本机IP
	}	
	//初始化地址结构体参数
	this->sockLenth=sizeof(this->addr);
}

CHostAddress::CHostAddress( const CHostAddress &host_addr )
{
	memcpy(&this->addr,&host_addr.addr,sizeof(host_addr.addr));
	memcpy(&this->ip,&host_addr.ip,sizeof(host_addr.ip));
	this->sockLenth=host_addr.sockLenth;
	this->port=host_addr.port;
}
char * CHostAddress::GetIp()
{
	return this->ip;
}

unsigned short CHostAddress::GetPort()
{
	return this->port;
}

struct sockaddr * CHostAddress::GetAddr()
{
	return ((struct sockaddr *)(&(this->addr)));
}

socklen_t CHostAddress::GetLen()
{
	return ((socklen_t)this->sockLenth);
}

socklen_t * CHostAddress::GetLenPtr()
{
	return ((socklen_t *)&(this->sockLenth));
}