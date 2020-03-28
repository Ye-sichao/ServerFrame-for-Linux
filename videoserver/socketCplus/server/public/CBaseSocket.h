#ifndef CBASESOCKET_H_
#define  CBASESOCKET_H_
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include "CHostAddress.h"
class CBaseSocket  //���������þ����Ȳ���һ���׽��֣��������ĳ�ʼ�׽��֣����߿ͻ��˵������׽���
{
public:
	CBaseSocket(int domain=PF_INET,int type=SOCK_STREAM,int protocol=0);
	CHostAddress GetAddr();  //δ����BIND������ΪNULL��������BIND�����ֵ
	bool Bind(CHostAddress host_addr); //�������˵ĵڶ������裬�ͻ�����û��
	int GetSocketFd();//��ȡ�׽���������
	bool SetSocketFd(int fd);
	bool CloseFd();
protected:
	void Create(int domain=PF_INET,int type=SOCK_STREAM,int protocol=0);
	int sock_fd;
	CHostAddress *host_addr;

};
#endif