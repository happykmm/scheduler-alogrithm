#include "cls_pm.h"



void initPm(clsstpmPtr THIS)
{
	pthis=THIS;

	pthis->getCountOfVMs=getCountOfVMs;
	pthis->getVMS=getVMS;
	pthis->set_fr_gr_id = set_fr_gr_id;
}

/*
 * @fn int PM::getCountOfVMs() const
 * Get the count of vm that hosted by this pm
 * @created by tuantuan at 2011.8.31
 */
int getCountOfVMs() 
{
	int i;int tot=0;
	
	/*for(i=0;i<MAX_VM_COUNT_PER_PM;i++)
		if(pthis->pm.vmlist[i].host.id!=-1) tot++;*/

    return tot;
}




/*���ص�ǰPM��VM����*/
int getVMS(){
	int i,j,atpid,totvm;
	vmPtr p_vm;


	totvm=pthis->sd->vm_total;
	j=0;
	for(i=0;i<totvm;i++)
	{
		p_vm=&pthis->sd->vmlist[i];
		atpid=p_vm->at;
		if(pthis->pm.host.id==atpid)
		{
			pthis->pm.p_vms[j]=p_vm;	
			j++;
		}
	}

	return j;
}

/*ȫ�ֱ���*/
BOOL clearpmlist()
{
	return 0;	
}

/*
������:set_fr_gr_id
���ܣ���ȡ�ַ�����ʽ��PM���ڿ�����Ϣ����������PM���ڵĿ����ָ��
Create by Liu Yuanqing
*/
void set_fr_gr_id()
{
	int i,j;
	int pm_id;
	int count;
	for(i=0;i<pthis->sd->pm_total;i++)
		pthis->sd->pmlist[i].countofpmgrp = 0;
	for(i=0;i<pthis->sd->fr_total;i++)
	{
		for(j=0;j<pthis->sd->framelist[i].countofpm;j++)
		{
			pm_id = pthis->sd->framelist[i].i_pmids[j];
			pthis->sd->pmlist[pm_id].at_p_fr = &pthis->sd->framelist[i];
		}
	}
	for(i=0;i<pthis->sd->gr_total;i++)
	{
		for(j=0;j<pthis->sd->pmglist[i].countofpm;j++)
		{
			pm_id = pthis->sd->pmglist[i].i_pmids[j];
			count = pthis->sd->pmlist[pm_id].countofpmgrp++;
			pthis->sd->pmlist[pm_id].at_p_pmgrps[count] = &pthis->sd->pmglist[i];
		}
	}
}


/*------------------------------------------------------*/
/*
 * @fn void newpm()
 * create a new pm
 * @created by jianhai chen at 2013.8.19
 */
//void newpm()
//{
//	int rp,tot_pm,ip3,ip4;
//	st_pm pm;
//	createPMV();
//	
//	rp=4*random_021();/*������4�������ѡ��һ����Ϊģ�帴��������PM*/
//				
//	tot_pm=pthis->sd->pm_total;
//
//	/*����һ��PM����VM����*/
//	//pm=PMV[(int)rp];
//				
//	pm.host.id=tot_pm;
//	pm.host.stat=0;
//	sprintf(pm.host.sid, "pm%d",tot_pm);
//
//	/*����IP*/
//	ip3=(int)(random_021()*255);
//	ip4=(int)(random_021()*255);
//	sprintf(pm.host.ip,"10.10.%d.%d",ip3,ip4);
//	pm.countofvm=0;
//
//	/*��PM����PMLIST*/
//	pthis->sd->pmlist[tot_pm]=pm;
//}
//



