#ifndef CCLIENTSOCKET_H_
#define CCLIENTSOCKET_H_
#include "CTcpSocket.h"
class CClientSocket :public CTcpSocket
{
public:
	CClientSocket(CHostAddress host_addr,int domain=PF_INET,int type=SOCK_STREAM,int protocol=0);

};

#endif