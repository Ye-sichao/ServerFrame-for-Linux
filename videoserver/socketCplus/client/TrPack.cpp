#include "TrPack.h"
void Packaging( void *buf,char *Data,int Packtype,int packlenth)
{
	HEAD head;
	TAIL tail;
	memset(&head,0,HEADSIZE);
	memset(&tail,0,TAILSIZE);
	head.head_remask=Head_value;
	tail.tail_remask=Tail_value;

	head.Pack_type=Packtype;
	head.Pack_size=packlenth;
	memcpy(Data,&head,HEADSIZE);
	memcpy(Data+HEADSIZE,buf,packlenth);
	memcpy(Data+HEADSIZE+packlenth,&tail,TAILSIZE);
}
/**********************************
*********
*Function: // Unpacking
*Description: // �������
******************************/
int Unpacking( char *buf ,char *Data,int *Packtype)
{
	HEAD head;
	TAIL tail;
	memset(&head,0,HEADSIZE);
	memset(&tail,0,TAILSIZE);
	memcpy(&head,buf,HEADSIZE);
	memcpy(&tail,buf+HEADSIZE+head.Pack_size,TAILSIZE);
	//���жϰ���������
	if(head.head_remask!=Head_value && tail.tail_remask!=Tail_value)
	{
		cout<<"pack uncomplete !"<<endl;
		return -1;
	}
	//���ذ�������
	*Packtype=head.Pack_type;
	//ȡ���������ݷ��ص���
	memcpy(Data,buf+HEADSIZE,head.Pack_size);
	return 0;
	
}
void MemPack(int packtype,DEALPACK *mempack,int fd,char *buf,int packlen)
{
	mempack->fd=fd;
	mempack->pack_type=packtype;
	memcpy(mempack->pack_msg,buf+HEADSIZE,packlen);
	mempack->pck_Lenth=packlen;
}