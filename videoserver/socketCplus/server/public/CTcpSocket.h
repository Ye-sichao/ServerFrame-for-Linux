#ifndef  CTCPSOCKET_H_
#define  CTCPSOCKET_H_
#include "CBaseSocket.h"
#include "CHostAddress.h"
#include <string.h>
#include <sys/types.h>
class CTcpSocket : public CBaseSocket  //这个类是CBaseSocket的第一个子类，主要是后几部的操作以及一个关于客户端的操作函数connect
{
public:
	CTcpSocket();
	CTcpSocket(int domain,int type,int protocol=0);
	CTcpSocket(CHostAddress host_addr,int domain,int type,int protocol=0);
	CHostAddress GetRemoteAddr(); //获取远程链接过来的客户端的IP地址和端口号
	void SetRemoteAddr(CHostAddress remote_addr);
	void Connect(CHostAddress host_addr);
	void Read(char *buf,int lenth);
	void Write(char *buf,int lenth);
protected:
	CHostAddress *pro_addr;
	CHostAddress remote_addr; //若是服务器用于保存链接的客户端的IP和端口号



};
#endif