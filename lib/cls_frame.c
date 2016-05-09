#include "cls_frame.h"
/*
函数名:initframe
参数说明:
THIS:cls_st_frame类结构体对象地址
功能：初始化cls_st_frame类结构体对象
Create by Liu Yuanqing
*/
void initframe(clsstframePtr THIS)
{
	_fthis = THIS;
	THIS->sid2ids = fr_sid2ids;
	THIS->get_frame_pmids = get_frame_pmids;
	THIS->getfrbysid = getfrbysid;
}
/*
函数名:get_frame_pmids()
功能：生成所有框包含的PM列表
Create by Liu Yuanqing
*/
void get_frame_pmids()
{
	int frame_total;
	int i;
	frame_total = _fthis->sd->fr_total;
	for(i=0;i<frame_total;i++)
		_fthis->sid2ids(i);
}
/*
函数名:fr_sid2ids
参数说明：
frame_id:框的ID
功能：生成ID为frame_id的框包含的PM列表
Create by Liu Yuanqing
*/
void fr_sid2ids(int frame_id)
{
	cls_st_host hh;
	char t_pmids[500];
	char *strpm[100];
	int countofpm;
	int i;
	initHost(&hh);
	hh.sd =_fthis->sd;
	strcpy(t_pmids,_fthis->sd->framelist[frame_id].pmids);
	countofpm=split_str(t_pmids,",",strpm);
	_fthis->sd->framelist[frame_id].countofpm = countofpm;
	for(i=0;i<countofpm;i++)
		_fthis->sd->framelist[frame_id].i_pmids[i] = hh.getpmbysid(strpm[i])->host.id; 
}

framePtr getfrbysid(char *sid)
{
	int i;
	framePtr p_fr=NULL;
	if(strlen(sid)==0) return NULL;
	for(i=0;i<_fthis->sd->fr_total;i++)
	{
		if(strcmp(sid,_fthis->sd->framelist[i].sid)==0){
			//找到了
			p_fr=&_fthis->sd->framelist[i];
			break;
		}
	}
	
	return p_fr;
}