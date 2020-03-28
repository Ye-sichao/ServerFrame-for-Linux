#include "CServer.h"
#include "CPostProcess.h"
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include "./sqlite3/qdata.h"
using namespace std;
//使用IO复用实现一个进程处理多个套接口
int main(int argc,char *argv[])
{
	int p_id;
	
	p_id=fork();
	if(p_id==0)
	{
			CPostProcess postp;
	}
	else if(p_id>0)
	{
		CServer server(1024);
		server.Start();
	}
/*	char **presult;
	int row,col;
	Qdata::getMydata()->gettable("select * from tbl_user;",presult,row,col );
	cout<<presult[4];*/
}