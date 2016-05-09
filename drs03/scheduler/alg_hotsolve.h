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

/*��Ԫ������ģ�ǵ��߼���Ԫ512VM���漰300����塣
Ŀǰ������һ����Ԫ��13�����*28����壬
����364�߼��塣��������20��*13��=260�߼��塣*/

#define HOTPM 1
#define WARMPM 0
#define COLDPM -1


/*�����ݽṹ���ڴ�Ž�ά��������������used*/
typedef struct{
	int id;
	double used;
} st_pm_1D;

/*�����ݽṹ���ڴ�Ŵ������������id��Դ�����id*/
typedef struct{
	int id;
	int pm_src_id;
} st_vm_wait;

typedef struct{
	
	int trans_num;	/*���ڼ�¼ת�Ʋ��Ե�����*/
	int hot_vm;		/*���ڼ�¼�������������̨��*/
	float hot,cold;/*cjh add*/
	st_pm_1D pm_1d_list[PM_TOTAL];
	st_vm_wait vm_wait[VM_TOTAL];

	sharedataPtr sd; /*ָ��ȫ�����ݹ�������ָ��*/
	
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
	st_vm_group * (*selectVMG)(pmPtr pm, int load);/*��ѡ������������Ǩ�Ƶ��������*/
	void (*action_unpackvm)(int vmg_tot, vmgPtr vmg);
	void (*swap_pm)(pmPtr pm1, pmPtr pm2);
	
} cls_st_hotsolve;

typedef cls_st_hotsolve* cls_hotsolvePtr;

static cls_hotsolvePtr _hsthis;

void init_hotsolve(cls_st_hotsolve *THIS); /*1-��ʼ��*/
void setRequest_hotsolve(cls_st_hotsolve *THIS);/*��ʼ�����������*/

void hotsolve();/*2-���ؾ��������ں���*/
st_vm_group * selectVMG(pmPtr pm, int load);/*��ѡ������������Ǩ�Ƶ��������*/
void action_unpackvm(int vmg_tot, vmgPtr vmg);/*���޷����õ����������actionlist*/
void swap_pm(pmPtr pm1, pmPtr pm2);/*//������̨�����*/

/*| | | | | | | | | | | | | | | | | | | | | | | | | | | | | ���º������� | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | */
int if_allhot(void);/*���������״̬�жϺ���*/
//int if_onehot(int pmID);/*���������״̬�жϺ���*/
void hot_hotsolve(int pmID, int load_num);/*hot�����hotsolve����*/
void warm_hotsolve(int pmID, int load_num);/*warm�����hotsolve����*/
void cold_hotsolve(int pmID, int load_num);/*cold�����hotsolve����*/
int hot_update(int pm_src_id,int pmID,int vmID);/*���������*/
void hot_store(int vmid, int pm_src_id, int pm_dst_id);/*���������v2p����*/
void output(void);/*������*/
void hot_pack(int vmID);/*װ��*/
void hot_pm(int *,int *);/*Ѱ���ȵ������*/


/*/////////////////////////////////////////////////////////////*/

#endif

