#ifndef CHOSTADDRESS_H_
#define  CHOSTADDRESS_H_
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
using namespace std;
class CHostAddress   //这个类是用来管理和保存IP地址和端口号的类
{
public:
	CHostAddress(const char *ip=NULL,unsigned short port=7777);
	CHostAddress(const CHostAddress &host_addr);
	bool SetIp(const char *ip);
	bool SetPort(unsigned short port);
	char * GetIp();
	unsigned short GetPort();
	struct sockaddr *GetAddr();
	socklen_t GetLen();
	socklen_t * GetLenPtr();

private:
	struct sockaddr_in addr;
	int sockLenth;
	char ip[32];
	unsigned short port;

};

#endif