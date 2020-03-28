#ifndef CSERVERSOCKET_H_
#define CSERVERSOCKET_H_
#include "CTcpSocket.h"
#include "CHostAddress.h"
#include <iostream>
using namespace std;
class CServerSocket : public CTcpSocket
{
public:
	CServerSocket(CHostAddress host_addr,int domain=PF_INET,int type=SOCK_STREAM,int protocol=0);
	void Listen();
	int Accept();
private:
	int Max_Connect_count;
	int Client_socket;

};















#endif