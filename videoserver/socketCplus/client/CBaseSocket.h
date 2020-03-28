#ifndef CBASESOCKET_H_
#define  CBASESOCKET_H_
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include "CHostAddress.h"
class CBaseSocket  //这个类的作用就是先产生一个套接字，服务器的初始套接字，或者客户端的链接套接字
{
public:
	CBaseSocket(int domain=PF_INET,int type=SOCK_STREAM,int protocol=0);
	CHostAddress GetAddr();  //未调用BIND，返回为NULL，调用了BIND后就有值
	bool Bind(CHostAddress host_addr); //服务器端的第二个步骤，客户端中没有
	int GetSocketFd();//获取套接字描述符
	bool SetSocketFd(int fd);
	bool CloseFd();
protected:
	void Create(int domain=PF_INET,int type=SOCK_STREAM,int protocol=0);
	int sock_fd;
	CHostAddress *host_addr;

};
#endif