#ifndef TRPACK_H_
#define TRPACK_H_
#include <iostream>
#include <string.h>
using namespace std;
void Packaging(  void *buf,char *Data,int Packtype,int packlenth);

int Unpacking(char *buf ,char *Data,int *Packtype);
	
#define Head_value 3  //包头标识
#define Tail_value 4   //包尾标识
#define Login_Type 1    //登录包类型
#define LoginAns_Type 2  //登录回应包
#define Revlist_Type 5  //视屏列表请求包
#define Rsvlist_Type 6   //视屏列表回应包
#define  Revpck_Type 7   //视屏详细信息请求包
#define Rsvpck_Type 8 //视屏详细信息回应包
#define ReVPlay_Type 14 //视屏播放请求包类型
#define RsVPlay_Type 15  //视屏播放回应包类型

//影片分类类型
#define  AnimeType 9  //动漫类型  
#define  MovieType 10 //电影类型
#define MusicType 11 //音乐类型
#define VarietyType 12 //综艺类型
#define  GameType  13//游戏类型

typedef struct Online_List_t //在线列表
{
	int sock_fd;
	char Name[64];
}Onlinelist;

typedef struct dealpack_t  //共享内存处理
{
	int fd;
	int pack_type;
	int pck_Lenth;
	char pack_msg[1400];
	

}DEALPACK;

typedef struct head_t    //包头
{
	int Pack_type;
	long int Pack_size;
	int head_remask;
}HEAD;

typedef struct login_t  //登录请求包
{
	char username[64];
	char paswd[32];
}LOGINPCK;

typedef struct loginans_t  //登录回应包
{
	bool is_ok;  

}LOGINANS;

typedef struct logout_t   //请求退出包
{
	int videoNo;
}EXITPCK;

typedef struct reslogout_t  //退出回应包
{
	
}EXITANS;

typedef struct tail_t
{
	int tail_remask;

}TAIL;

typedef struct req_Video_t  //视屏详细信息请求包
{
	int CurType;
}REVPCK;

typedef struct video_msg_t  //视屏详细信息
{
	char imagePath1[64];
	char imagePath2[64];
	char imagePath3[64];
	char MvName[64];
	char VideoPath[64];
	long cout;//播放次数
}VIDEOMSG;

typedef struct rsp_Video_t  //视屏详细信息回应包
{
	int video_count;
	VIDEOMSG videomsg[5];
}RSVPCK;

typedef struct req_play_t  //视屏点播请求包
{
	int video_num;       //根据这个包记录某个视屏的点播数量，以及生成用户的历史播放影片
	char username[64];

}PLAYREQ;

typedef struct rsp_play_t   //视屏点播回应包
{
	char Mvname[64];       //返回影片名字以及影片图片路径供客户端历史记录页面显示
	char image_path[64];
}PLAYRSP;





void MemPack(int packtype,DEALPACK *mempack,int fd,char *buf,int packlen);
#define DEALPCSIZE sizeof(DEALPACK)
#define  HEADSIZE sizeof(HEAD)
#define TAILSIZE sizeof(TAIL)
#define LISTSIZE sizeof(Onlinelist)
#define LOGINANSSIZE sizeof(LOGINANS) 
#endif