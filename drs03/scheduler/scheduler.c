
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
@函数名称：init_sched_algorithm
@Initialize the scheduler to do scheduling
@参数：
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
@函数名称：do_initplace
@------Implement initplace by scheduling the initplace algorithm 
@------in alg_initplace class
@参数：
@Created by jianhaichen at 2013-05-16
@modified by #### at ####-##-##
****************************************************/
void do_initplace()
{
	cls_st_vm_group vmg;
	cls_st_initplace ini;
	int i;
	//cls_initplacePtr p_initplc;

	/*流程1）分组；2）放置与切割；3）输出放置结果*/
	/*1)分组*/
	
	init_vmgroup(&vmg);	
	vmg.sd=_scthis->sd;
	
	/*基于流量分成大组*/	
	vmg.generate_VM_group(_scthis->sd->vmglist);

	/*输出组*/
//	vmg.output_parts(_scthis->sd->vmglist);
	
	/*2）放置*/
	//-进行初试放置
	printf("开始放置决策，进行组的装箱......\n");
	/*vm_group_packing(); packing之后结果存放在part数组中*/	
	
	init_initplace(&ini);
	ini.sd=_scthis->sd;
	setRequest_initplace(&ini);
	
	/*初始化配置参数 cjh add 08.21*/
	//setRequest_initplace(&ini);

	ini.initplace_by_vmgroup(_scthis->sd->vmglist);  /*执行放置*/
	

	
	/*3）输出放置结果*/
	//-输出分组结果
	printf("输出装箱的放置决策结果......\n");
	/*输出组*/
//	vmg.output_parts(_scthis->sd->vmglist);

	/*4)输出组到action列表中*/
	vmg.output_actionlist(_scthis->sd->vmglist);


}

/****************************************************
@函数名称：do_consolidate
@------执行热点消除
@------调用算法set
@参数：
@Created by jianhaichen at 2013-05-16
@modified by #### at ####-##-##
****************************************************/

void do_hotsolve()
{	
	cls_st_hotsolve hsl;
	cls_st_vm_group vmg;

	
	init_vmgroup(&vmg);	
	vmg.sd=_scthis->sd;
	
	/*基于流量分成大组*/	
	vmg.generate_VM_group(_scthis->sd->vmglist);

	hsl.sd=_scthis->sd;
	init_hotsolve(&hsl);/*初始化调度算法类*/
	setRequest_hotsolve(&hsl);/*初始化请求参数hot,cold*/
	hsl.hotsolve();/*调度热点消除入口*/
	printf("hotsolve successfully......\n");
}


/****************************************************
@函数名称：do_loadbalance
@------执行负载均衡
@------调用算法
@参数：
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
	
	/*基于流量分成大组*/	
	vmg.generate_VM_group(_scthis->sd->vmglist);
	lb.sd = _scthis->sd;
	init_loadbalance(&lb);
	
	setRequest_LB(&lb);/*设置公共参数hot,cold,balance,...,cjh add 08.21*/
	
	lb.balance = 0; /*???????cjh???*/
	lb.loadbalance();//修改为调用loadbalance()函数，by sun in 8-15
}


/****************************************************
@函数名称：do_consolidate  ==== do_coldsolve
@------执行服务器整合
@------调用算法
@参数：
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
	
	/*基于流量分成大组*/	
	vmg.generate_VM_group(_scthis->sd->vmglist);
	csl.sd=_scthis->sd;
	init_coldsolve(&csl);
	
	setRequest_coldsolve(&csl);

	csl.coldsolve(csl.hot,csl.cold,csl.warm);

	printf("Coldsolve successfully......\n");
}



/****************************************************
@函数名称：ask_netctrl_for_net_bw
@向网络控制器请求带宽约束满足，
@参数：
@返回：成功返回1，否则返回0
@Created by jianhaichen at 2013-05-16
@modified by #### at ####-##-##
****************************************************/
int ask_netctrl_for_net_bw(vmgPtr p_pvmg, pmPtr p_pm){
	int ret=1;
	
	
	return ret;
}

#endif

