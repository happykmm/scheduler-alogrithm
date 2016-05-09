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
/*分组算法的类结构体*/
typedef struct{
	/************************************************/
	/*****属性定义***************************/
	sharedataPtr sd;/*共享数据结构指针*/	

	int grp_number;/*分组个数*/

	/*************************************************/
	/*类内部公共函数*/
			
	/*1.生成虚拟机组函数指针*/
	void (*generate_VM_group)(vmgPtr ppart);

	/*2.输出分组信息*/
	void (*output_parts)(vmgPtr ppart);
	
	void (*output_actionlist)(vmgPtr ppart);

	/*组的切割函数*/
	double (*group_cut)(int part_no,vmgPtr ppart);	

	/*反亲和性检查*/
	//void (*check_aff)(int part_no,vmgPtr ppart);

	/*对ID为pm_id的物理机内的VM分组*/
	void (*grouping_in_pm)(int pm_id);

	/*获取vmg对应的id字符串 cjh add 08.26*/
	void (*getVMidstrByvmg)(int vmgid,char *ret);
	
} cls_st_vm_group;

/*定义类对象的指针*/
typedef cls_st_vm_group* cls_vmgroupPtr;

/*指向类对象的指针*/
cls_vmgroupPtr _gcthis; 



/**********************2.内部属性与临时数据结构*****************************/

int visit[VM_TOTAL];/*深度优先搜索访问标志*/
//int visit_group[VM_TOTAL];//对组进行深度优先搜索访问标志

int grp_number;/*分组个数*/
//int grp_group_number;/*分组继续分组后的组编号*/
int g_edge_count;/*实际所有的边的个数*/
int in_set[VM_TOTAL];/*记录切割后是否在组内的临时变量*/
int sub[VM_TOTAL];/*记录最终切割结果*/
int ini_afflist_total;
st_vm2vm ini_afflist[V2V_TOTAL];
double dimMax[DIM];

/*临时数据结构*/ 
st_adj_vmid_list g_adj_vmid_list[MAX_VM_COUNT_PER_GROUP]; /*邻接表*/
//st_adj_part_list g_adj_part_list[VM_TOTAL]; /*组邻接表*/

////////////////////////////////////////////////////////////////////////////


/**********************1.对外调用的函数*****************************/
/*初始化分组类指针*/
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
/*生成虚拟机流量关系组*/
void generate_VM_group(vmgPtr ppart);

/*输出分组信息*/
void output_parts(vmgPtr ppart);

void output_actionlist(vmgPtr ppart);

void grouping_in_pm(int pm_id);

/**********************3.对内调用的函数*****************************/


/*@功能:创建虚拟机邻接表*/
void create_adj_vm_list();

/*初始化分组数组信息*/
void init_part(vmgPtr ppart);

/*深度优先搜索算法*/
void dfs(int x,int part_no, vmgPtr ppart, int vm_id_list[],int dimLoad[]);

/*分组主函数，生成结果在ppart指向的分组数组g_part中*/
void part_component(vmgPtr ppart);

/*给定一个组号，将其所有的点对应的边的号找出来,加入到part数组*/
void set_edge(vmgPtr ppart,int part_no);

/*求解亲和组的总负载，为各虚拟机的负载之和*/
void set_part_load(vmgPtr ppart);
//extern void vm_group_packing();

/*获取x到y的距离*/
st_adj_list_node *get_listnode(int x,int y);

/*获取当前组内的流量信息*/
void get_traffic(int part_no);

/*S-W算法中求WAGE并得到S-T最小割的函数*/
double search (int *ps,int *pt,int part_no,int vm_total);

/*Stoer-Wagner算法求最小割，以实现对任意物理机都装不下的虚拟机组的切割 */
double group_cut(int part_no,vmgPtr ppart);

/*针对不同板的切割*/
double group_cut2(int part_no,vmgPtr ppart,int src,int dst);

/*反亲和性检查*/
//void check_aff(int part_no,vmgPtr ppart);

/*DFS检查反亲和性*/
double dfs_check(int x,int dst,int min_src,int min_dst,double flow);

/*删除邻接表中src到dst及dst到src的信息*/
void delete_edge(int src,int dst);

/*获取元素在表中的ID*/
int get_id_in_list(int element,int total, int list[]);

void create_adj_vm_list_in_pm(int pm_id);

int get_id_in_p_vms(int vm_id,int vm_total,vmPtr p_vms[]);

void dfs2(int x,int part_no, vmgPtr ppart,vmPtr p_vms[]);

void get_afflist();

/*获取vmg对应的id字符串 cjh add 08.26*/
void getVMidstrByvmg(int vmgid,char *ret);

#endif


