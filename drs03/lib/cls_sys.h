/*
 * @file cls_sys.h includes sys definitions functions to call publicly
 * ����ϵͳ�Ĺ��������࣬��������ϵͳ��״̬��ϵͳ���帺�صȺ���
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

	/*�ж��ȵ㣬Ѱ���ȵ�*/
	BOOL (*JudgeHot)(int pmID,double hot);
	BOOL (*FindHotPm)(int *pmID, int *load,double hot);

	/*���߱�����actionlist*/
	void (*SetActionlist) (int action, pmPtr pm1, pmPtr pm2, vmPtr vm);

	/*��Ǩ�ƾ���д��actionlist*/
	void (*migrate_actionlist)(vmgPtr *vmg, cls_old_at * old_atlist, int n);

	/*��������Ƴ������*/
	void (*getVMoutofPM)(vmPtr vm, pmPtr pm);

	/*���������ȡ�������*/
	void (*getVMGoutofPM)(vmgPtr vmg, pmPtr pm);

	/*�Ѿ�����ĳ��vmg���õ�pm*/
	void (*moveVMGtoPM)(vmgPtr vmg, pmPtr pm);

	/*�Ѿ�����ĳ��vm���õ�pm*/
	void (*moveVMtoPM)(vmPtr vm, pmPtr pm);

	/*��ȡÿ�������ά�����ֵ*/
	double* (*get_max_vec) (double vec_max[ ]);

	/*��һ��*/
	double (*normalization) (st_host * ptr, int dgree, double max[ ]);
	
	/*����Դ��ά*/
	double (*down_1dgree_tot) (int id, char sign);
	
	/*ռ�ý�ά*/
	double (*down_1dgree_use) (int id);
	
	/*���������*/
	void (*sort1) ( );
	void (*sort2) ( );
	
	/*��ȡϵͳ״̬*/
	int (*get_sys_stat) (double hot,double cold, double warm );

	/*����vmgԭ�����������id*/
	void(*save_vmg_oldat) (int n_vmg, vmgPtr vmg);


	void (*sort) (int *bg, int *ed, int (*cmp)(int a,int b));

	void (*placeVMGtoPM)(vmgPtr *vmgs, int n_vmg, pmPtr *pms, int n_pms, vm2pmPtr ret,int* n_ret);
	int (*checkPlaceAff)(vmgPtr vmg, pmPtr pm);
	int (*can_be_packed)(int part_no,int pm_no, vmgPtr ppart);

	void (*random_shuffle)(unsigned char *begin, unsigned char *end, int size);

	int (*getMinAvailCountOfVmg)(vmgPtr *p_vmgs,int n_vmg,int *order);

	/*��vmg��vm�ĸ��ؽ������*/
	void (*SumVMG)(int);

	int (*getSysNonEmptyPM);

	int maxSearchDepth;

} cls_st_sys;

typedef cls_st_sys* clsstsysPtr;

clsstsysPtr sthis;

//��������Ҫ�õ��ĸ�������

char same_group[VM_TOTAL];//�����ͬ���VMG

char is_kernel[VM_TOTAL];//�Ƿ�Ϊ������ͻ��Ľ���

char need_place[VM_TOTAL];//�Ƿ���Ҫ�ڵ�ǰ�����з���

char dfs_flag[VM_TOTAL];//DFS���Ƿ��ѱ�����

double dimMax[DIM];

//��������vmg����
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

/*�ж��ȵ㣬Ѱ���ȵ�*/
BOOL JudgeHot(int pmID);
BOOL FindHotPm(int *pmID, int *load);

/*���߱�����actionlist*/
void SetActionlist (int action, pmPtr pm1, pmPtr pm2, vmPtr vm);

/*��Ǩ�ƾ���д��actionlist*/
void migrate_actionlist(vmgPtr *vmg, cls_old_at * old_atlist, int n);

/*��vmgroup���õ�pm�������׺��Եķ��÷���*/
void placeVMGtoPM(vmgPtr *vmgs, int n_vmg, pmPtr *pms, int n_pms, vm2pmPtr ret,int* n_ret);

/*���ĳ�η����Ƿ����㱻����pm�����ڵķ��׺���*/
int checkPlaceAff(vmgPtr vmg, pmPtr pm);

//�������ٿɷ�����������������ID
int getMinAvailCountOfVmg(vmgPtr *p_vmgs,int n_vmg,int *order);

//ͨ�ÿ���������
void sort(int *bg, int *ed, int (*cmp)(int a,int b));


int can_be_packed(int part_no,int pm_no, vmgPtr ppart);

void random_shuffle(unsigned char *begin, unsigned char *end, int size);

/*�Ѿ�����ĳ��vmg���õ�pm*/
void moveVMGtoPM(vmgPtr vmg, pmPtr pm);

/*�Ѿ�����ĳ��vm���õ�pm*/
void moveVMtoPM(vmPtr vm, pmPtr pm);

/*�������ȡ�������*/
void getVMoutofPM(vmPtr vm, pmPtr pm);

/*���������ȡ�������*/
void getVMGoutofPM(vmgPtr vmg, pmPtr pm);

/*�������������������������б�*/
void setVMLIST(vmPtr vm, vmgPtr vmg);

/* by jiajunwei  */

/*��ø���ά�ȵ����ֵ*/
double* get_max_vec (double vec_max[ ]);

/*��һ������*/
double normalization (st_host * ptr, int dgree, double max[ ]);

/*��ά*/
double down_1dgree_tot (int id, char sign);
double down_1dgree_use (int id);

/*����*/
void sort1 ( );
void sort2 ( );

/*�ж�ϵͳ״̬*/
int get_sys_stat(double hot,double cold, double warm);

void save_vmg_oldat(int n_vmg, vmgPtr vmg);

/*��vmg��vm�ĸ��ؽ������*/
void SumVMG(int);
#endif
