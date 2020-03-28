drop table tbl_user;
drop table tbl_vdrecord;
drop table tbl_video;
drop table tbl_anime;
drop table tbl_variety;
drop table tbl_music;
drop table tbl_game;
drop table tbl_movie;
create table tbl_user      --�û���
(
	user_id integer(16) primary key,
	user_fd integer(10) not null,
	user_name varchar(10) not null, --�������룬�´ε�¼ʱҪ��֤	
	user_pswd varchar(10) not null
);


create table tbl_vdrecord    --ӰƬ��¼��
(
	user_name varchar(64) primary key, --��ʶ����ӰƬ��¼������һ���û�
	video_no integer(5) not null,
	played_time integer(50) default 0	 
);

create table tbl_video      --ӰƬ��  ӰƬID ӰƬ���� ����ͼƬ�ڿͻ�����·�� ӰƬ�Ĳ���Դλ�� 
(
	video_id integer(10) primary key,
	video_name varchar(30) not null,
	picture1_path varchar(64) not null,
	picture2_path varchar(64) not null,
	picture3_path varchar(64) not null,
	video_path varchar(64) not null,
	video_count integer(1) default 0
);
										   --���ҳ����ڶ�����ӰƬ��Ƭ��
create table tbl_anime    --����ӰƬ��     
(
	video_id integer(10) primary key
);
 --�����ѯ���ʾ��  select video_name from tbl_video as a,tbl_anime as b where a.video_id=b.video_id; 
create table tbl_movie    --��ӰӰƬ��
(
	video_id integer(10) primary key

);

create table tbl_variety   --����ӰƬ��
(
	video_id integer(10) primary key

);

create table tbl_music     --���־�� 
(

	video_id integer(10) primary key

);

create table tbl_game      --��Ϸֱ����
(
	video_id integer(10) primary key

);






