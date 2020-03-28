#ifndef TRPACK_H_
#define TRPACK_H_
#include <iostream>
#include <string.h>
using namespace std;
void Packaging(  void *buf,char *Data,int Packtype,int packlenth);

int Unpacking(char *buf ,char *Data,int *Packtype);
	
#define Head_value 3  //��ͷ��ʶ
#define Tail_value 4   //��β��ʶ
#define Login_Type 1    //��¼������
#define LoginAns_Type 2  //��¼��Ӧ��
#define Revlist_Type 5  //�����б������
#define Rsvlist_Type 6   //�����б��Ӧ��
#define  Revpck_Type 7   //������ϸ��Ϣ�����
#define Rsvpck_Type 8 //������ϸ��Ϣ��Ӧ��
#define ReVPlay_Type 14 //�����������������
#define RsVPlay_Type 15  //�������Ż�Ӧ������

//ӰƬ��������
#define  AnimeType 9  //��������  
#define  MovieType 10 //��Ӱ����
#define MusicType 11 //��������
#define VarietyType 12 //��������
#define  GameType  13//��Ϸ����

typedef struct Online_List_t //�����б�
{
	int sock_fd;
	char Name[64];
}Onlinelist;

typedef struct dealpack_t  //�����ڴ洦��
{
	int fd;
	int pack_type;
	int pck_Lenth;
	char pack_msg[1400];
	

}DEALPACK;

typedef struct head_t    //��ͷ
{
	int Pack_type;
	long int Pack_size;
	int head_remask;
}HEAD;

typedef struct login_t  //��¼�����
{
	char username[64];
	char paswd[32];
}LOGINPCK;

typedef struct loginans_t  //��¼��Ӧ��
{
	bool is_ok;  

}LOGINANS;

typedef struct logout_t   //�����˳���
{
	int videoNo;
}EXITPCK;

typedef struct reslogout_t  //�˳���Ӧ��
{
	
}EXITANS;

typedef struct tail_t
{
	int tail_remask;

}TAIL;

typedef struct req_Video_t  //������ϸ��Ϣ�����
{
	int CurType;
}REVPCK;

typedef struct video_msg_t  //������ϸ��Ϣ
{
	char imagePath1[64];
	char imagePath2[64];
	char imagePath3[64];
	char MvName[64];
	char VideoPath[64];
	long cout;//���Ŵ���
}VIDEOMSG;

typedef struct rsp_Video_t  //������ϸ��Ϣ��Ӧ��
{
	int video_count;
	VIDEOMSG videomsg[5];
}RSVPCK;

typedef struct req_play_t  //�����㲥�����
{
	int video_num;       //�����������¼ĳ�������ĵ㲥�������Լ������û�����ʷ����ӰƬ
	char username[64];

}PLAYREQ;

typedef struct rsp_play_t   //�����㲥��Ӧ��
{
	char Mvname[64];       //����ӰƬ�����Լ�ӰƬͼƬ·�����ͻ�����ʷ��¼ҳ����ʾ
	char image_path[64];
}PLAYRSP;





void MemPack(int packtype,DEALPACK *mempack,int fd,char *buf,int packlen);
#define DEALPCSIZE sizeof(DEALPACK)
#define  HEADSIZE sizeof(HEAD)
#define TAILSIZE sizeof(TAIL)
#define LISTSIZE sizeof(Onlinelist)
#define LOGINANSSIZE sizeof(LOGINANS) 
#endif