/*

 
*/


#ifndef _DRS_LOADBALANCE_H
#define _DRS_LOADBALANCE_H

#include "../lib/drsdata.h"
#include "../lib/drsdef.h"
#include <math.h>
#include "../lib/cls_sys.h"
#include "algorithm.h"
//struct cls_st_loadbalance;

/*网元的最大规模是单逻辑网元512VM，涉及300物理板。
目前场景有一个网元含13物理框*28物理板，
共计364逻辑板。场景二：20框*13板=260逻辑板。*/

//st_pm pm_bin_list[500]; /*作为待放置的PM列表*/
//st_vm vm_list[512];/*逻辑板个数*/
#define LB_COUNT 1000
typedef struct{

	sharedataPtr sd; /*指向全局数据共享区的指针*/
	
	void (*loadbalance)(); /*调度函数*/
	//公共参数
	double balance, cold, hot;
	int maxcount;/*add by cjh 08.21*/

	//6项权值
	double cpu_w, mem_w, io_w, eth0_w, eth1_w, eth2_w;

	//全集群上6项的负载均值
	double avgcpu, avgmem, avgio, avgeth0, avgeth1, avgeth2;

	//list
	//VMList vmList;
    //PMList pmList;

	//每个pm的load，6项总和
    double pmload[PM_TOTAL];

	double vmgload[VM_TOTAL];
	int vmgorder[VM_TOTAL];
	double avg[DIM];
	double weight[DIM];
	int trans_num;
	/*int setWeight(double cpuw, double memw, double iow, double eth0w, double eth1w, double eth2w);*/


	void (*Calculate)();
	
} cls_st_loadbalance;

typedef cls_st_loadbalance* cls_loadbalancePtr;

static cls_loadbalancePtr _lbthis;
	cls_st_sys css;
	cls_st_vm_group cvg;
void init_loadbalance(cls_st_loadbalance *THIS); /*初始化*/

/*设置参数*/
void setRequest_LB(cls_loadbalancePtr THIS);/*add by cjh 08.21*/

/*负载均衡调度入口函数*/
void loadbalance();
double get_remained(pmPtr ppm,int dim);
void setDefaultArgs();
double pmBalance(double cpuw , double memw , 
				 double iow , double eth0w , 
				 double eth1w, double eth2w );
int calculatePMbyLoad();
int selectPMbyLoad(int flag);
int checkVMGtoPM(int vmname,int pmname);
int selectVMGfromPM(int pmname, int flag);
double calculatePMAfter(int pmname,int vmname, int flag);
int removeVMGfromPM(int pmname,int vmname);
int addVMGtoPM(int pmname,int vmname);
int selectPMforVMG(int pmname_o,int vmname);
int addAction(int pmname, int pmnew, int vmname);
void Calculate();
void appendMigrAct(int vmid, int pm_src_id, int pm_dst_id);
void _lb_output();
/*/////////////////////////////////////////////////////////////*/

#endif

