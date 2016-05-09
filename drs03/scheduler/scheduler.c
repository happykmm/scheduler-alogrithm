
/*
 * @file drs_algorithm.h drs algorithm base class
 *
 * @modified by tuantuan at 2011.8.19
 */
#ifndef _SCHED_H
#define _SCHED_H
#include "scheduler.h"
#include "../lib/drslib.h"
/****************************************************
@�������ƣ�init_sched_algorithm
@Initialize the scheduler to do scheduling
@������
@Created by jianhaichen at 2013-05-16
@modified by #### at ####-##-##
****************************************************/
/**/
void init_scheduler(cls_schedulerPtr THIS){
	
	_scthis=THIS;

	THIS->initplace=do_initplace;
    THIS->hotsolve=do_hotsolve;
	THIS->loadbalance=do_loadbalance;
	THIS->consolidation=do_consolidate;
	srand(0);


}

/****************************************************
@�������ƣ�do_initplace
@------Implement initplace by scheduling the initplace algorithm 
@------in alg_initplace class
@������
@Created by jianhaichen at 2013-05-16
@modified by #### at ####-##-##
****************************************************/
void do_initplace()
{
	cls_st_vm_group vmg;
	cls_st_initplace ini;
	int i;
	//cls_initplacePtr p_initplc;

	/*����1�����飻2���������и3��������ý��*/
	/*1)����*/
	
	init_vmgroup(&vmg);	
	vmg.sd=_scthis->sd;
	
	/*���������ֳɴ���*/	
	vmg.generate_VM_group(_scthis->sd->vmglist);

	/*�����*/
//	vmg.output_parts(_scthis->sd->vmglist);
	
	/*2������*/
	//-���г��Է���
	printf("��ʼ���þ��ߣ��������װ��......\n");
	/*vm_group_packing(); packing֮���������part������*/	
	
	init_initplace(&ini);
	ini.sd=_scthis->sd;
	setRequest_initplace(&ini);
	
	/*��ʼ�����ò��� cjh add 08.21*/
	//setRequest_initplace(&ini);

	ini.initplace_by_vmgroup(_scthis->sd->vmglist);  /*ִ�з���*/
	

	
	/*3��������ý��*/
	//-���������
	printf("���װ��ķ��þ��߽��......\n");
	/*�����*/
//	vmg.output_parts(_scthis->sd->vmglist);

	/*4)����鵽action�б���*/
	vmg.output_actionlist(_scthis->sd->vmglist);


}

/****************************************************
@�������ƣ�do_consolidate
@------ִ���ȵ�����
@------�����㷨set
@������
@Created by jianhaichen at 2013-05-16
@modified by #### at ####-##-##
****************************************************/

void do_hotsolve()
{	
	cls_st_hotsolve hsl;
	cls_st_vm_group vmg;

	
	init_vmgroup(&vmg);	
	vmg.sd=_scthis->sd;
	
	/*���������ֳɴ���*/	
	vmg.generate_VM_group(_scthis->sd->vmglist);

	hsl.sd=_scthis->sd;
	init_hotsolve(&hsl);/*��ʼ�������㷨��*/
	setRequest_hotsolve(&hsl);/*��ʼ���������hot,cold*/
	hsl.hotsolve();/*�����ȵ��������*/
	printf("hotsolve successfully......\n");
}


/****************************************************
@�������ƣ�do_loadbalance
@------ִ�и��ؾ���
@------�����㷨
@������
@Created by jianhaichen at 2013-05-16
@modified by #### at ####-##-##
****************************************************/

void do_loadbalance()
{
	cls_st_loadbalance lb;
	int i;
	cls_st_vm_group vmg;

	
	init_vmgroup(&vmg);	
	vmg.sd=_scthis->sd;
	
	/*���������ֳɴ���*/	
	vmg.generate_VM_group(_scthis->sd->vmglist);
	lb.sd = _scthis->sd;
	init_loadbalance(&lb);
	
	setRequest_LB(&lb);/*���ù�������hot,cold,balance,...,cjh add 08.21*/
	
	lb.balance = 0; /*???????cjh???*/
	lb.loadbalance();//�޸�Ϊ����loadbalance()������by sun in 8-15
}


/****************************************************
@�������ƣ�do_consolidate  ==== do_coldsolve
@------ִ�з���������
@------�����㷨
@������
@Created by jianhaichen at 2013-05-16
@modified by #### at ####-##-##
****************************************************/

void do_consolidate()
{
	//float hot,cold,warm;
	cls_st_coldsolve csl;
	cls_st_vm_group vmg;

	
	init_vmgroup(&vmg);	
	vmg.sd=_scthis->sd;
	
	/*���������ֳɴ���*/	
	vmg.generate_VM_group(_scthis->sd->vmglist);
	csl.sd=_scthis->sd;
	init_coldsolve(&csl);
	
	setRequest_coldsolve(&csl);

	csl.coldsolve(csl.hot,csl.cold,csl.warm);

	printf("Coldsolve successfully......\n");
}



/****************************************************
@�������ƣ�ask_netctrl_for_net_bw
@������������������Լ�����㣬
@������
@���أ��ɹ�����1�����򷵻�0
@Created by jianhaichen at 2013-05-16
@modified by #### at ####-##-##
****************************************************/
int ask_netctrl_for_net_bw(vmgPtr p_pvmg, pmPtr p_pm){
	int ret=1;
	
	
	return ret;
}

#endif

