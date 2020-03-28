#include "CClientSocket.h"

CClientSocket::CClientSocket( CHostAddress host_addr,int domain,int type,int protocol/*=0*/ )
:CTcpSocket(host_addr,domain,type,protocol)
{
	//初始化地址完成，就直接往服务器发送链接请求
	this->Connect(host_addr);

}