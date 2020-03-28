drop table tbl_user;
drop table tbl_vdrecord;
drop table tbl_video;
drop table tbl_anime;
drop table tbl_variety;
drop table tbl_music;
drop table tbl_game;
drop table tbl_movie;
create table tbl_user      --用户表
(
	user_id integer(16) primary key,
	user_fd integer(10) not null,
	user_name varchar(10) not null, --保存密码，下次登录时要验证	
	user_pswd varchar(10) not null
);


create table tbl_vdrecord    --影片记录表
(
	user_name varchar(64) primary key, --标识这条影片记录属于哪一个用户
	video_no integer(5) not null,
	played_time integer(50) default 0	 
);

create table tbl_video      --影片表  影片ID 影片名字 封面图片在客户机的路径 影片的播放源位置 
(
	video_id integer(10) primary key,
	video_name varchar(30) not null,
	picture1_path varchar(64) not null,
	picture2_path varchar(64) not null,
	picture3_path varchar(64) not null,
	video_path varchar(64) not null,
	video_count integer(1) default 0
);
										   --查找出属于动漫的影片的片名
create table tbl_anime    --动漫影片表     
(
	video_id integer(10) primary key
);
 --链表查询语句示例  select video_name from tbl_video as a,tbl_anime as b where a.video_id=b.video_id; 
create table tbl_movie    --电影影片表
(
	video_id integer(10) primary key

);

create table tbl_variety   --综艺影片表
(
	video_id integer(10) primary key

);

create table tbl_music     --音乐剧表 
(

	video_id integer(10) primary key

);

create table tbl_game      --游戏直播表
(
	video_id integer(10) primary key

);






