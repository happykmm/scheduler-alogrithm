/*

 
*/


#ifndef _DRS_hotsolve_H
#define _DRS_hotsolve_H

#include "../lib/drsdata.h"
#include "../lib/drsdef.h"
#include "../lib/cls_sys.h"
#include "../lib/cls_vmgroup.h"
#include <stdio.h>

//struct cls_st_hotsolve;

/*网元的最大规模是单逻辑网元512VM，涉及300物理板。
目前场景有一个网元含13物理框*28物理板，
共计364逻辑板。场景二：20框*13板=260逻辑板。*/

#define HOTPM 1
#define WARMPM 0
#define COLDPM -1


/*该数据结构用于存放降维后的物理机，及其used*/
typedef struct{
	int id;
	double used;
} st_pm_1D;

/*该数据结构用于存放待放置虚拟机的id和源物理机id*/
typedef struct{
	int id;
	int pm_src_id;
} st_vm_wait;

typedef struct{
	
	int trans_num;	/*用于记录转移策略的条数*/
	int hot_vm;		/*用于记录待放置虚拟机的台数*/
	float hot,cold;/*cjh add*/
	st_pm_1D pm_1d_list[PM_TOTAL];
	st_vm_wait vm_wait[VM_TOTAL];

	sharedataPtr sd; /*指向全局数据共享区的指针*/
	
	void (*hotsolve)(); 
//	void (*transto_1D)();
//	void (*sort_pm)();
	int (*if_allhot)();
//	int (*if_onehot)(int);
	void (*hot_hotsolve)(int, int);
	void (*warm_hotsolve)(int, int );
	void (*cold_hotsolve)(int, int );
	int (*hot_update)(int, int, int );
	void (*hot_store)(int, int, int);
	void (*output)();
	void (*hot_pack)(int);
	void (*hot_pm)(int *,int *);
	st_vm_group * (*selectVMG)(pmPtr pm, int load);/*挑选物理机上最合适迁移的虚拟机组*/
	void (*action_unpackvm)(int vmg_tot, vmgPtr vmg);
	void (*swap_pm)(pmPtr pm1, pmPtr pm2);
	
} cls_st_hotsolve;

typedef cls_st_hotsolve* cls_hotsolvePtr;

static cls_hotsolvePtr _hsthis;

void init_hotsolve(cls_st_hotsolve *THIS); /*1-初始化*/
void setRequest_hotsolve(cls_st_hotsolve *THIS);/*初始化请求传入参数*/

void hotsolve();/*2-负载均衡调度入口函数*/
st_vm_group * selectVMG(pmPtr pm, int load);/*挑选物理机上最合适迁移的虚拟机组*/
void action_unpackvm(int vmg_tot, vmgPtr vmg);/*将无发放置的虚拟机存入actionlist*/
void swap_pm(pmPtr pm1, pmPtr pm2);/*//交换两台物理机*/

/*| | | | | | | | | | | | | | | | | | | | | | | | | | | | | 以下函数不用 | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | */
int if_allhot(void);/*物理机整体状态判断函数*/
//int if_onehot(int pmID);/*物理机单个状态判断函数*/
void hot_hotsolve(int pmID, int load_num);/*hot情况下hotsolve函数*/
void warm_hotsolve(int pmID, int load_num);/*warm情况下hotsolve函数*/
void cold_hotsolve(int pmID, int load_num);/*cold情况下hotsolve函数*/
int hot_update(int pm_src_id,int pmID,int vmID);/*更新物理机*/
void hot_store(int vmid, int pm_src_id, int pm_dst_id);/*将结果存入v2p数组*/
void output(void);/*输出结果*/
void hot_pack(int vmID);/*装箱*/
void hot_pm(int *,int *);/*寻找热点物理机*/


/*/////////////////////////////////////////////////////////////*/

#endif

