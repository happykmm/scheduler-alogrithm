
/*
 * @file drs_algorithm.h drs algorithm base class
 *
 * @modified by tuantuan at 2011.8.19
 */
#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "algorithm.h"


typedef struct{
	
	sharedataPtr sd;

	void (*initplace)();             /*do initplace*/
	void (*hotsolve)();             /*do hotsolve */
	void (*loadbalance)();             /*do loadbalance */
	void (*consolidation)();             /*do consolidation */
	
	/*������������������Լ�����㣬�ɹ�����1�����򷵻�0*/
	int (*ask_netctrl_for_net_bw)(vmgPtr p_pvmg, pmPtr p_pm);
} cls_st_scheduler;

typedef cls_st_scheduler* cls_schedulerPtr; /*�����㷨��d*/

static cls_schedulerPtr _scthis;

void init_scheduler(cls_schedulerPtr THIS);

void do_initplace();/*do Initplace function*/
void do_hotsolve(); /*do do_hotsolve function*/
void do_loadbalance();/*do do_loadbalance function*/
void do_consolidate();/*do do_consolidate function*/

#endif

