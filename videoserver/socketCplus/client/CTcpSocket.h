#ifndef  CTCPSOCKET_H_
#define  CTCPSOCKET_H_
#include "CBaseSocket.h"
#include "CHostAddress.h"
#include <string.h>
#include <sys/types.h>
class CTcpSocket : public CBaseSocket  //�������CBaseSocket�ĵ�һ�����࣬��Ҫ�Ǻ󼸲��Ĳ����Լ�һ�����ڿͻ��˵Ĳ�������connect
{
public:
	CTcpSocket();
	CTcpSocket(int domain,int type,int protocol=0);
	CTcpSocket(CHostAddress host_addr,int domain,int type,int protocol=0);
	CHostAddress GetRemoteAddr(); //��ȡԶ�����ӹ����Ŀͻ��˵�IP��ַ�Ͷ˿ں�
	void SetRemoteAddr(CHostAddress remote_addr);
	void Connect(CHostAddress host_addr);
	void Read(char *buf,int lenth);
	void Write(char *buf,int lenth);
protected:
	CHostAddress *pro_addr;
	CHostAddress remote_addr; //���Ƿ��������ڱ������ӵĿͻ��˵�IP�Ͷ˿ں�



};
#endif