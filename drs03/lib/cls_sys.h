/*
 * @file cls_sys.h includes sys definitions functions to call publicly
 * 调度系统的公共处理类，计算整个系统的状态、系统整体负载等函数
 * @created by jianhaichen <chenjh919@zju.edu.cn>, zhejiang university and huwei l.t.d
 * @modified by jianhaichen at 2013.4.12
 */

#ifndef TDRS_CLS_SYS_H_INCLUDED
#define TDRS_CLS_SYS_H_INCLUDED

#include "drsdata.h"
#include "util.h"
#include "cls_host.h"
#include <stdlib.h>

typedef struct{
	int id;
	double tot;
	double total_used;
} num;

typedef struct {
	int id;
	int old_at;
}cls_old_at;

typedef struct{
	cls_st_host host;
	sharedataPtr sd;
	
	num exm1[PM_TOTAL];
	cls_old_at old_atlist[5000];
	
	int n_pms;
	pmPtr *pms;
	
	double hot,cold,warm;/*cjh add 08.24*/
	
	/* System stat warm cold hot 0 -1 1*/
    int systemStat;



	/*----Functions-----*************************************************/
	/* Get the total used of the whole system with the specified metric */
    double (*getSysTotalUsed)(int name);

    /* Get the average used of the whole system with the specified metric */
    double (*getSysAvgUsed)(int name);

    /* Get the average used of the whole system with the specified metric */
    double (*getSysTotalHave)(int name);

    /* 
     * Calculate the current balance degree of the whole sys,
     * It's a common function for each algorithm
     */
    double (*getSysScore)( int flag);

    double (*getSysTotalLoad)();

    double (*getSysAvgLoad)();
 
    double (*getSysBalance)();

    
    /* 
     * Calculate one of the PM's several metric balance, 
     * to make sure of each the resouce be used nearly
     */
    double (*getPMSkw)(pmPtr pm);


    /* get the volume of the Host*/
    double (*getVOL)(hostPtr hptr);
    double (*getVSR)(pmPtr hptr);
    double (*getAddVOL)(pmPtr hptr);

    /* System state process functions */
    int (*setSysStat)(int stat);

    int (*getSysStatRange)();

    /* Get the resource usage percent*/
    double (*getPercent)(hostPtr pm, int name, int type);

	/*判断热点，寻找热点*/
	BOOL (*JudgeHot)(int pmID,double hot);
	BOOL (*FindHotPm)(int *pmID, int *load,double hot);

	/*决策保存入actionlist*/
	void (*SetActionlist) (int action, pmPtr pm1, pmPtr pm2, vmPtr vm);

	/*将迁移决策写入actionlist*/
	void (*migrate_actionlist)(vmgPtr *vmg, cls_old_at * old_atlist, int n);

	/*将虚拟机移出物理机*/
	void (*getVMoutofPM)(vmPtr vm, pmPtr pm);

	/*将虚拟机组取出物理机*/
	void (*getVMGoutofPM)(vmgPtr vmg, pmPtr pm);

	/*已决定将某个vmg放置到pm*/
	void (*moveVMGtoPM)(vmgPtr vmg, pmPtr pm);

	/*已决定将某个vm放置到pm*/
	void (*moveVMtoPM)(vmPtr vm, pmPtr pm);

	/*获取每个物理机维度最大值*/
	double* (*get_max_vec) (double vec_max[ ]);

	/*归一化*/
	double (*normalization) (st_host * ptr, int dgree, double max[ ]);
	
	/*总资源降维*/
	double (*down_1dgree_tot) (int id, char sign);
	
	/*占用降维*/
	double (*down_1dgree_use) (int id);
	
	/*物理机排序*/
	void (*sort1) ( );
	void (*sort2) ( );
	
	/*获取系统状态*/
	int (*get_sys_stat) (double hot,double cold, double warm );

	/*保存vmg原本所在物理机id*/
	void(*save_vmg_oldat) (int n_vmg, vmgPtr vmg);


	void (*sort) (int *bg, int *ed, int (*cmp)(int a,int b));

	void (*placeVMGtoPM)(vmgPtr *vmgs, int n_vmg, pmPtr *pms, int n_pms, vm2pmPtr ret,int* n_ret);
	int (*checkPlaceAff)(vmgPtr vmg, pmPtr pm);
	int (*can_be_packed)(int part_no,int pm_no, vmgPtr ppart);

	void (*random_shuffle)(unsigned char *begin, unsigned char *end, int size);

	int (*getMinAvailCountOfVmg)(vmgPtr *p_vmgs,int n_vmg,int *order);

	/*对vmg内vm的负载进行求和*/
	void (*SumVMG)(int);

	int (*getSysNonEmptyPM);

	int maxSearchDepth;

} cls_st_sys;

typedef cls_st_sys* clsstsysPtr;

clsstsysPtr sthis;

//函数中需要用到的辅助数组

char same_group[VM_TOTAL];//标记相同组的VMG

char is_kernel[VM_TOTAL];//是否为两个冲突组的交点

char need_place[VM_TOTAL];//是否需要在当前任务中放置

char dfs_flag[VM_TOTAL];//DFS中是否已被遍历

double dimMax[DIM];

//待搜索的vmg队列
vmgPtr queue_vmg[VM_TOTAL];
char inqueue[VM_TOTAL];
int availPMcount[VM_TOTAL];
int queue_N;


void initSys(clsstsysPtr THIS);

int getSysNonEmptyPM();

/* Get the total used of the whole system with the specified metric */
double getSysTotalUsed(int name);

/* Get the average used of the whole system with the specified metric */
double getSysAvgUsed(int name);

/* Get the average used of the whole system with the specified metric */
double getSysTotalHave(int name);

/* 
    * Calculate the current balance degree of the whole sys,
    * It's a common function for each algorithm
    */
double getSysScore(int flag);
double getSysTotalLoad();

//double getSysTotalLoad(AIMFP fp, const PMList &pmList);
//double getSysAvgLoad(AIMFP fp, const PMList &pmList);
//double getSysBalance(AIMFP fp, const PMList &pmList);
double getSysBalance();
double getSysAvgLoad();
    
/* 
    * Calculate one of the PM's several metric balance, 
    * to make sure of each the resouce be used nearly
    */
double getPMSkw(pmPtr pm);


/* get the volume of the Host*/
double getVOL(hostPtr hptr);
double getVSR(pmPtr hptr);
double getAddVOL(pmPtr hptr);

/* System state process functions */
int setSysStat(int stat);

int getSysStatRange();

/* Get the resource usage percent*/
double getPercent(hostPtr hptr, int name, int type);

/*判断热点，寻找热点*/
BOOL JudgeHot(int pmID);
BOOL FindHotPm(int *pmID, int *load);

/*决策保存入actionlist*/
void SetActionlist (int action, pmPtr pm1, pmPtr pm2, vmPtr vm);

/*将迁移决策写入actionlist*/
void migrate_actionlist(vmgPtr *vmg, cls_old_at * old_atlist, int n);

/*将vmgroup放置到pm，符合亲和性的放置方案*/
void placeVMGtoPM(vmgPtr *vmgs, int n_vmg, pmPtr *pms, int n_pms, vm2pmPtr ret,int* n_ret);

/*检查某次放置是否满足被放置pm框、组内的反亲和性*/
int checkPlaceAff(vmgPtr vmg, pmPtr pm);

//返回最少可放置物理机的虚拟机组ID
int getMinAvailCountOfVmg(vmgPtr *p_vmgs,int n_vmg,int *order);

//通用快速排序函数
void sort(int *bg, int *ed, int (*cmp)(int a,int b));


int can_be_packed(int part_no,int pm_no, vmgPtr ppart);

void random_shuffle(unsigned char *begin, unsigned char *end, int size);

/*已决定将某个vmg放置到pm*/
void moveVMGtoPM(vmgPtr vmg, pmPtr pm);

/*已决定将某个vm放置到pm*/
void moveVMtoPM(vmPtr vm, pmPtr pm);

/*将虚拟机取出物理机*/
void getVMoutofPM(vmPtr vm, pmPtr pm);

/*将虚拟机组取出物理机*/
void getVMGoutofPM(vmgPtr vmg, pmPtr pm);

/*将虚拟机组放入带放置虚拟机组列表*/
void setVMLIST(vmPtr vm, vmgPtr vmg);

/* by jiajunwei  */

/*获得各个维度的最大值*/
double* get_max_vec (double vec_max[ ]);

/*归一化处理*/
double normalization (st_host * ptr, int dgree, double max[ ]);

/*降维*/
double down_1dgree_tot (int id, char sign);
double down_1dgree_use (int id);

/*排序*/
void sort1 ( );
void sort2 ( );

/*判断系统状态*/
int get_sys_stat(double hot,double cold, double warm);

void save_vmg_oldat(int n_vmg, vmgPtr vmg);

/*对vmg内vm的负载进行求和*/
void SumVMG(int);
#endif
