#include "CClientSocket.h"

CClientSocket::CClientSocket( CHostAddress host_addr,int domain,int type,int protocol/*=0*/ )
:CTcpSocket(host_addr,domain,type,protocol)
{
	//��ʼ����ַ��ɣ���ֱ����������������������
	this->Connect(host_addr);

}