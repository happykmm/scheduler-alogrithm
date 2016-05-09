#include "cls_frame.h"
/*
������:initframe
����˵��:
THIS:cls_st_frame��ṹ������ַ
���ܣ���ʼ��cls_st_frame��ṹ�����
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
������:get_frame_pmids()
���ܣ��������п������PM�б�
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
������:fr_sid2ids
����˵����
frame_id:���ID
���ܣ�����IDΪframe_id�Ŀ������PM�б�
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
			//�ҵ���
			p_fr=&_fthis->sd->framelist[i];
			break;
		}
	}
	
	return p_fr;
}