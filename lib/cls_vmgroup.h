/*
 * @file alg_groupcut.h algorithm groupcut case
 * @created by jianhaichen at 2013-05-13
 */
#ifndef _ALG_GROUPCUT_H
#define _ALG_GROUPCUT_H
//#include "alg_initplace.h"
#include "drsdata.h"
#include "drsdef.h"
#include "drslib.h"
#include "util.h"
#include <stdio.h>
//#include "../scheduler/alg_coldsolve.h"
#define INF 999999
#define TRUE 1
#define FALSE 0
#define NOT_IN_GROUP -1
#define UNCONNECTED 999999
#define MIN(x,y) (((x)<(y))?(x):(y))
extern char nameOfMetric[DIM][20];

//g_v2vlist
/*�����㷨����ṹ��*/
typedef struct{
	/************************************************/
	/*****���Զ���***************************/
	sharedataPtr sd;/*�������ݽṹָ��*/	

	int grp_number;/*�������*/

	/*************************************************/
	/*���ڲ���������*/
			
	/*1.����������麯��ָ��*/
	void (*generate_VM_group)(vmgPtr ppart);

	/*2.���������Ϣ*/
	void (*output_parts)(vmgPtr ppart);
	
	void (*output_actionlist)(vmgPtr ppart);

	/*����и��*/
	double (*group_cut)(int part_no,vmgPtr ppart);	

	/*���׺��Լ��*/
	//void (*check_aff)(int part_no,vmgPtr ppart);

	/*��IDΪpm_id��������ڵ�VM����*/
	void (*grouping_in_pm)(int pm_id);

	/*��ȡvmg��Ӧ��id�ַ��� cjh add 08.26*/
	void (*getVMidstrByvmg)(int vmgid,char *ret);
	
} cls_st_vm_group;

/*����������ָ��*/
typedef cls_st_vm_group* cls_vmgroupPtr;

/*ָ��������ָ��*/
cls_vmgroupPtr _gcthis; 



/**********************2.�ڲ���������ʱ���ݽṹ*****************************/

int visit[VM_TOTAL];/*��������������ʱ�־*/
//int visit_group[VM_TOTAL];//���������������������ʱ�־

int grp_number;/*�������*/
//int grp_group_number;/*�����������������*/
int g_edge_count;/*ʵ�����еıߵĸ���*/
int in_set[VM_TOTAL];/*��¼�и���Ƿ������ڵ���ʱ����*/
int sub[VM_TOTAL];/*��¼�����и���*/
int ini_afflist_total;
st_vm2vm ini_afflist[V2V_TOTAL];
double dimMax[DIM];

/*��ʱ���ݽṹ*/ 
st_adj_vmid_list g_adj_vmid_list[MAX_VM_COUNT_PER_GROUP]; /*�ڽӱ�*/
//st_adj_part_list g_adj_part_list[VM_TOTAL]; /*���ڽӱ�*/

////////////////////////////////////////////////////////////////////////////


/**********************1.������õĺ���*****************************/
/*��ʼ��������ָ��*/
void init_vmgroup(cls_vmgroupPtr THIS);

/*function:generate group from vmlist
  parameters:
     input: 
		p_vmlist, point to the input global vmlist;
		p_vm_id, point to the initial vm_id_list for grouping vms.
	 output:
	    p_vm_id: point a result grouped vm_id list
  created by jianhai Chen, 05-06-2013
*/
/*���������������ϵ��*/
void generate_VM_group(vmgPtr ppart);

/*���������Ϣ*/
void output_parts(vmgPtr ppart);

void output_actionlist(vmgPtr ppart);

void grouping_in_pm(int pm_id);

/**********************3.���ڵ��õĺ���*****************************/


/*@����:����������ڽӱ�*/
void create_adj_vm_list();

/*��ʼ������������Ϣ*/
void init_part(vmgPtr ppart);

/*������������㷨*/
void dfs(int x,int part_no, vmgPtr ppart, int vm_id_list[],int dimLoad[]);

/*���������������ɽ����ppartָ��ķ�������g_part��*/
void part_component(vmgPtr ppart);

/*����һ����ţ��������еĵ��Ӧ�ıߵĺ��ҳ���,���뵽part����*/
void set_edge(vmgPtr ppart,int part_no);

/*����׺�����ܸ��أ�Ϊ��������ĸ���֮��*/
void set_part_load(vmgPtr ppart);
//extern void vm_group_packing();

/*��ȡx��y�ľ���*/
st_adj_list_node *get_listnode(int x,int y);

/*��ȡ��ǰ���ڵ�������Ϣ*/
void get_traffic(int part_no);

/*S-W�㷨����WAGE���õ�S-T��С��ĺ���*/
double search (int *ps,int *pt,int part_no,int vm_total);

/*Stoer-Wagner�㷨����С���ʵ�ֶ������������װ���µ����������и� */
double group_cut(int part_no,vmgPtr ppart);

/*��Բ�ͬ����и�*/
double group_cut2(int part_no,vmgPtr ppart,int src,int dst);

/*���׺��Լ��*/
//void check_aff(int part_no,vmgPtr ppart);

/*DFS��鷴�׺���*/
double dfs_check(int x,int dst,int min_src,int min_dst,double flow);

/*ɾ���ڽӱ���src��dst��dst��src����Ϣ*/
void delete_edge(int src,int dst);

/*��ȡԪ���ڱ��е�ID*/
int get_id_in_list(int element,int total, int list[]);

void create_adj_vm_list_in_pm(int pm_id);

int get_id_in_p_vms(int vm_id,int vm_total,vmPtr p_vms[]);

void dfs2(int x,int part_no, vmgPtr ppart,vmPtr p_vms[]);

void get_afflist();

/*��ȡvmg��Ӧ��id�ַ��� cjh add 08.26*/
void getVMidstrByvmg(int vmgid,char *ret);

#endif


