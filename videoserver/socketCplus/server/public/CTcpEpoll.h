#include "CEpollBase.h"
#include "CHostAddress.h"
#include "CServerSocket.h"
class CTcpEpoll :public CEpollBase
{
public:
	CTcpEpoll(int maxevents);
	~CTcpEpoll();
	void Run(int ncount);
	virtual void Ondata(int fd,char *buf);
	bool SetAddr(CHostAddress addr);
protected:
	CHostAddress addr;
	CServerSocket *Tcpserver;

};