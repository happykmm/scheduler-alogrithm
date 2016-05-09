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

/*��Ԫ������ģ�ǵ��߼���Ԫ512VM���漰300����塣
Ŀǰ������һ����Ԫ��13�����*28����壬
����364�߼��塣��������20��*13��=260�߼��塣*/

//st_pm pm_bin_list[500]; /*��Ϊ�����õ�PM�б�*/
//st_vm vm_list[512];/*�߼������*/
#define LB_COUNT 1000
typedef struct{

	sharedataPtr sd; /*ָ��ȫ�����ݹ�������ָ��*/
	
	void (*loadbalance)(); /*���Ⱥ���*/
	//��������
	double balance, cold, hot;
	int maxcount;/*add by cjh 08.21*/

	//6��Ȩֵ
	double cpu_w, mem_w, io_w, eth0_w, eth1_w, eth2_w;

	//ȫ��Ⱥ��6��ĸ��ؾ�ֵ
	double avgcpu, avgmem, avgio, avgeth0, avgeth1, avgeth2;

	//list
	//VMList vmList;
    //PMList pmList;

	//ÿ��pm��load��6���ܺ�
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
void init_loadbalance(cls_st_loadbalance *THIS); /*��ʼ��*/

/*���ò���*/
void setRequest_LB(cls_loadbalancePtr THIS);/*add by cjh 08.21*/

/*���ؾ��������ں���*/
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

