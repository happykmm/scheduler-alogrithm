#include "cls_pmgroup.h"
void initPmgroup(clsstpmgroupPtr THIS)
{
	_pmgthis = THIS;
	//THIS->getCountPMs = getCountPMs;
	THIS->get_group_pmids = get_group_pmids;
	THIS->sid2ids = pmg_sid2ids;
	THIS->getpmgbysid = getpmgbysid;
}

/*
函数名:pmg_sid2ids
参数说明：
group_id:物理机组的ID
功能：生成ID为group_id的物理机组包含的PM列表
Create by Liu Yuanqing
*/
void pmg_sid2ids(int group_id)
{
	cls_st_host hh;
	char t_pmids[PMIDS_SIZE_GR];
	char *strpm[MAX_PMCOUNT_PER_GROUP];
	int countofpm;
	int i;
	initHost(&hh);
	hh.sd = _pmgthis->sd;
	strcpy(t_pmids,_pmgthis->sd->pmglist[group_id].pmids);
	countofpm=split_str(t_pmids,",",strpm);
	_pmgthis->sd->pmglist[group_id].countofpm = countofpm;
	for(i=0;i<countofpm;i++)
	_pmgthis->sd->pmglist[group_id].i_pmids[i] = hh.getpmbysid(strpm[i])->host.id; 
}

/*
函数名:get_group_pmids
功能：生成所有物理机组包含的PM列表
Create by Liu Yuanqing
*/
void get_group_pmids()
{
	int group_total;
	int i;
	group_total = _pmgthis->sd->gr_total;
	for(i=0;i<group_total;i++)
		_pmgthis->sid2ids(i);
}

pmgPtr getpmgbysid(char *sid)
{
	int i;
	pmgPtr p_pmg=NULL;
	if(strlen(sid)==0) return NULL;
	for(i=0;i<_pmgthis->sd->gr_total;i++)
	{
		if(strcmp(sid,_pmgthis->sd->pmglist[i].sid)==0){
			//找到了
			p_pmg=&_pmgthis->sd->pmglist[i];
			break;
		}
	}
	
	return p_pmg;
}