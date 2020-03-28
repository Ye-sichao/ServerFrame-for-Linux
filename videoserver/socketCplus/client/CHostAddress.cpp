#include "CHostAddress.h"
CHostAddress::CHostAddress( const char *ip,unsigned short port )
{
	//˽�����Ա���IP�Ͷ˿ںţ������ýṹ���ֵ,��սṹ���ֵ
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
	
	//���ýṹ��ĸ���Ԫ��
	this->addr.sin_family=PF_INET;//ʹ��IPV4
	this->addr.sin_port=htons(this->port);//����ת����˿ں�
	if(ip==NULL)
	{
		this->addr.sin_addr.s_addr=INADDR_ANY;
	}
	else
	{
		cout<<"ip save ok "<<this->ip<<endl;
		this->addr.sin_addr.s_addr=inet_addr(this->ip);//Ĭ���ǿͻ��˵�IP��������Ҫת��������Ƿ������ˣ������Զ�Ѱ�ұ���IP
	}	
	//��ʼ����ַ�ṹ�����
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