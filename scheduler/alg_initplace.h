/*
 初试放置原理： 
 输入一组VM虚拟机序列，每个VM具有一定的资源容量，将其放置到一组物理机中。
 1、同构异构问题
 1）同构下：目标在满足资源容量放得下的前提下，使物理机的数量尽可能少；
 2）异构下：
   （1）成本目标：在满足容量限制的情况下，使所耗物理机资源的成本最少；
   （2）性能目标：在满足容量限制的情况下，给定能够放得下的所耗物理机
   资源，各物理机能达到负载均衡；
 
 2、基本要求：
 1）虚拟机除满足自身负载资源需求（CPU、mem、磁盘IO），还要满足网络流量需求，
 VM之间存在通信，使相互通信的虚拟机能够集中放置——分组。
 2）物理机之间存在网络上的通信距离：距离的远近由经过的路由器节点的个数确定；
 部署的目标要求通信开销最小，即传输给定的数据，所有的时延最小（时延=数据量/带宽）
 3）部署原则：两个具有频繁通信的VM，被放置在两个不同的PM，要求选取PM时使PM之间的
 距离要最小。

 4）PM拓扑网络表示为一棵树，每个节点是PM或交换机。
 PM上不同的网络端口，可能被连接到不同的交换机，不同PM之间的网络端口，存在一条路径；
 路径的性能指标：路径时延，经过该路径上交换机的个数；用Link连接表示相邻两个端口之间的
 联接。

 3、相关算法：
 1）分组算法
 根据VM及关联，对VM进行分组；
 根据VM之间的亲和性，对VM进行分组；
 2）放置算法
 选择VM策略：
    随机策略：随机选取一个VM组，进行放置；
	大VM组先放置的策略：将VM组按容量需求从大到小排序，FFD；
	小VM组先放置的策略：将VM组按容量需求从小到大排序，进行放置；
 
 选择物理机的策略：
   最短距离策略：按照物理机的距离，选取一组距离相近的物理机；
   容量策略：选取能够放得下的物理机；
   优先放置小物理机策略，为能使到来的需求量的VM放得下，选择容量小的先放下；
 
 
 最优时延或最小东西流量的策略：
   1）分组；2）按组FFD；3）多个物理机按最短距离选取；4）将VM逐一放入PM；
 
*/


#ifndef _DRS_INITPLACE_H
#define _DRS_INITPLACE_H


#include "../lib/drsdata.h"
//#include "../lib/drsdef.h"
#include "../lib/drslib.h"
#define OVERLOAD -1
//#include "alg_groupcut.h"

//#include "drs_algorithm.h"


//struct _alg_initplace;

/*网元的最大规模是单逻辑网元512VM，涉及300物理板。
目前场景有一个网元含13物理框*28物理板，
共计364逻辑板。场景二：20框*13板=260逻辑板。*/

//st_pm pm_bin_list[500]; /*作为待放置的PM列表*/
//st_vm vm_list[512];/*逻辑板个数*/
typedef struct{

	sharedataPtr sd; /*指向全局数据共享区的指针*/

	vmgPtr part; /*指向分组的指针*/


	void (*initplace_by_vmgroup)(vmgPtr ppart);

	//void (*initplace)(); /*调度函数*/
	
	int (*check_naff)(int part_no,vmgPtr ppart,int vmg_total,int vmg_list[]);
} cls_st_initplace;

typedef struct{
	int dst_pm_id;
	double delay;
}st_delay;

typedef struct{
	double cpu,mem,diskio,fpga,eth0,eth1,eth2,eth3;
}st_load;

typedef cls_st_initplace* cls_initplacePtr;

static cls_initplacePtr _ithis;

double group_traffic[PM_TOTAL];            
int traffic_order_group[VM_TOTAL];
double pm_source[PM_TOTAL];   
vmgPtr p_vmg[VM_TOTAL];
//int pm_group[PM_TOTAL];
//int pm_num;


void init_initplace(cls_st_initplace *THIS); /*初始化*/
void setRequest_initplace(cls_initplacePtr THIS);


/*基于组的装箱初试放置算法调度函数*/
void initplace_by_vmgroup(vmgPtr ppart);

int can_place(int part_no,int pm_no,double hot[],vmgPtr ppart);

/*放置指定的part_no的组*/
BOOL pack_group(int part_no,double hot[],vmgPtr ppart);


/*初试放置调度入口函数*/
void initplace();

void compute_itraffic();//计算内部流量
void sort_by_traffic();
void find_min_star(int grp_id,int pm_total,int pm_list[]);
int get_available_pm(int grp_id,int pm_list[]);
int in_list(int host_id,int total,int host_list[]);
void init_at_pmid();
void set_at_pmid(int part_no,vmgPtr ppart);
int check_naff(int part_no,vmgPtr ppart,int vmg_total,int vmg_list[]);
void add_new_p_vmg(vmgPtr vmg);
void NP_place(vmgPtr *p_vmg,int vmg_total);
//extern int grp_number;

/*extern st_part_vm_group g_part[VM_TOTAL];*/


#endif

