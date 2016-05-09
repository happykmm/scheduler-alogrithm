
/*
 * @file drsdata.h includes data types, data structures and definitions
 * @created by jianhaichen <chenjh919@zju.edu.cn>, zhejiang university and huwei l.t.d
 * @modified by jianhaichen at 2013.4.12
 */

#ifndef TDRS_DATA_H_INCLUDED
#define TDRS_DATA_H_INCLUDED

/*debug*/
#define DEBUG 1

/* Host metric DIM*/
#define DIM 10 /*DIMs as 8*/
#define PORT_NUM 4 /*The total number of port in PM*/
#define SW_PORT_NUM 24 /*The total number of port in SWITCH*/

#define MAX_VM_COUNT_PER_GROUP 512
#define MAX_EDGE_PER_GROUP 512
#define MAX_GROUP_COUNT_PER_GROUP 512
#define MAX_VM_COUNT_PER_PM 100
#define MAX_GR_COUNT_PER_PM 10 /*每个PM最多属于逻辑组的个数*/
#define MAX_PMCOUNT_PER_FRAME 50/*每个框最多容纳的物理机个数*/
#define PMIDS_SIZE 50*33 /*框的物理机ID字符串总长度，每个PMID的字符串不超过10*/
#define PMIDS_SIZE_GR 4000*33 /*框的物理机ID字符串总长度，每个PMID的字符串不超过10*/
#define MAX_PMCOUNT_PER_GROUP 100/*每个框最多容纳的物理机个数*/

/******************************************************************/
#define VM_TOTAL 10000  /*VM的个数*/
#define PM_TOTAL 5000 /*pm COUNT , 10000 TO 5000 CJH MODIFY 7.28*/
#define V2V_TOTAL 100000 /*流量关系边的个数*/
#define P2P_TOTAL 10000 /*物理机之间的距离边的个数*/
#define V2P_TOTAL 10000 /*虚拟机与物理机放置关系个数*/
#define FR_TOTAL 1000 /*frame count*/
#define PM_GR_TOTAL 5000 /*group count 物理机分组的个数*/

/*---物理机拓扑构成---------------------*/
#define P2S_TOTAL 16000/*物理机与交换机连接的数量 lyq add 7.24*/
#define S2S_TOTAL 10000/*交换机与交换机连接的数量 lyq add 7.24*/
#define SW_TOTAL 450 /*交换机个数 CJH ADD 7.28*//*modified by lyq 8.18*/

/*---调度控制用-------------------------*/
#define REQ_TOTAL 100 /*REQUEST TOTAL 请求个数*/
#define ACT_TOTAL 100000 /*调度实施操作个数 cjh add 7.28*/

#define TREE_NODE_TOTAL 8000 /*pm COUNT about 4000 and switch 4000*/
#define NETLINK_TOTAL 100000 /*netlink COUNT about 10000*/

/*---调度算法与调度控制用---------------*/
#define TRUE 1
#define FALSE 0
#define BOOL int
#define MAXDELAY 32767
#define MAX_TRAFFIC 999999 /*表示流量大小无穷大的常量，用于处理同板亲和性*/
#define INVI -1
#define EPSINON 0.00001
#define DEFIP "10.10"
#define WAIT_VMG_TOTAL 2000

#define HOT 0.80
#define WALM 0.50
#define COLD 0.20


/******************************************************************/
/*denote ID of an object*/
typedef char OBJECTID[40];

/* Const definitions */
/* An enum that lists all metric names */
enum MetricName {CPU = 0, MEM, DISKIO, FPGA, ETH0, ETH1, ETH2, ETH3, SUM, MAX, MIN};
/* An enum that lists all metric value types */
enum MetricType { USED = 0, TOTAL, RESERVED, AVAIL };
/* An enum that lists all host state */
enum STAT { VMIGR = 0, VPLACE = 1, PCHECK, INVAL};
/* An enum that lists all node type in the PM network*/
enum NODE_TYPE { PM_NODE = 0, SWITCH_NODE = 1};

/*An enum that lists all actions for drs controller*/
enum controlActions {DOGENERATEDATA = 1, DOSCHEDULE = 2, DOPREDICT =3 };

/*An enum that lists all schedules for drs controller*/
enum ScheduleActions {DOINITPLACE = 1, DOLOADBALANCE = 2, DOCONSOLIDATE = 3, DOHOTSOLVE = 4 };

/*An enum that lists all types of scheduling algorithms*/
enum AlgotithmTypes {PLACE=1, LOADBALANCE=2, CONSOLIDATE=3, HOTSOLVE=4, PREDICT=5};

/*An enum that lists all type of objects to be packed*/
enum pack_object_type{T_PM=1, T_VM=2, T_VMG=3};

/*An enum that lists all type of affinity*/
enum affinity_type{N_AFF_GR=-3,N_AFF_FR=-2, N_AFF_PM=-1,NO_AFF=0, AFF_PM=1, AFF_FR=2, AFF_GR=3 };

enum LINK_TYPE{PMLINK=0, SWITCHLINK=1};

enum actiontype{ACTION_PLACE=0,ACTION_MIGR=1,ACTION_DOWN=2,ACTION_UP=3}; 

/*PM容量负载规格*/
typedef struct {
	double pcpu[4];
	double pmem[4];
	double pdisk[4];
	double pfpga[4];
	double pnet[4];
}st_PMResVector;

/*
 * @object Metric
 *
  FOR pm: 
  1) CPU: Total is the sum of core number, used is the real using cpu, reserved is reserved cpu for certain utility.
  2) MEM: total is the sum of all mem; used is all used  mem; reserved is reserved capacity for a special utility;
  3) FPGA: TOTAL is the number of FPGA board; used is used, reserved is reserved used;
  4) NET: TOTAL is 100%, bandwidth, 
 * Performance metric value: include used, total, reserved
 */
typedef struct {
    char metric_name[40];          // metric name

    double used;                   // used value
    double total;                  // total value
    double reserved;               // reserved value
	//char label[20];			   // 用于表示指标的类型 
} st_metric;
typedef st_metric* metricPtr;
/*--------------------------------------------------------------------------*/
/*Net element*/
typedef struct{
	int neID;
	char neName[40];
	int tenaID; //the vm's tenant.
} st_net_element;
typedef st_net_element* netelePtr;



/*物理机分组 2013.07.25CJH*/
typedef struct{
	int id;
	OBJECTID sid;
	char name[40];
	char *pmids;
	int i_pmids[PM_TOTAL];
	int countofpm;//物理机ID的个数
} st_pmgroup;
typedef st_pmgroup* pmgPtr;/*指向st_pm的指针*/



/*框 2013.07.25CJH*/
typedef struct{
	int id;
	OBJECTID sid;
	char name[30];
	char pmids[PMIDS_SIZE];//该框下的物理机集合，最多放置50个板，每个板的ID字符串不超过10个字符
	int i_pmids[50];/*一个框不超过50个PM*/
	int countofpm;//物理机ID的个数
} st_frame;
typedef st_frame* framePtr;/*指向st_pm的指针*/

/*超级组 2013.08.09LYQ*/
typedef struct{
	int id;
	int i_pmids[50];/*一个框不超过50个PM*/
	int countofpm;//物理机ID的个数
	pmgPtr at_p_pmgrps[10]; 
	int countofpmgrp;
	framePtr at_p_fr;
} st_hyper_group;
typedef st_hyper_group* hgPtr;/*指向st_pm的指针*/


/*--------------------------------------------------------------------------*/

typedef struct{
	int id;   /*Inner single coder*/
	OBJECTID sid; //denote string id or name for VM
	int link_port; /*连接的端口数*/
	char ip[30];/*用于外部交互的IP地址，表示唯一*/
	char eip[PORT_NUM][30]; /*this ip is used for eth*/ 
	int stat; /*主机状态*//*state：-表示待初试放置，此时，ovs_mac为空*/
	int dimRead;
	st_metric load[DIM]; /*loaday*, *denote vmCpu, vmMem, vmDiskio, vmFpga.*/
} st_host;
typedef st_host* hostPtr;

/*5)define the data structure of logic board
(which is also named virtual machine)*/
typedef struct{
	st_host host;
	st_net_element net_elm; //The net element which VM belongs to
	
	int at_vmgid;/*the vmgroup that the VM belongs to*/
	//int at_partid;

	int head; //指向afflist的链表头 下标

	int at; //<!—放置位置--> 原先VM放置的位置

	int at_pmid;//决策之后VM放置的位置

	int ask;/*临时增加，用于生成数据*/

	//int n_aff_fr_list[300];/*用于存放完整的不同框反亲和性信息*/
	//int n_aff_fr_total;

	int aff_pm_head,n_aff_pm_head,aff_fr_head,n_aff_fr_head,aff_gr_head,n_aff_gr_head;
	int at_frame_id,at_pmg_id;
} st_vm;

typedef st_vm* vmPtr;

/*--------------------------------------------------------------------------*/
typedef struct{
	st_host host;
	
	/*所在超级组*/
	hgPtr at_p_hg;
	/*所在组信息*/
	pmgPtr at_p_pmgrps[10]; /*一个物理机至多放在10个组内  CJH ADD 2013.07.25*/
	char at_gr_ids[10*33]; /*denotes which group the PM belongs to，一个PM可以放置在多个组*/
	int countofpmgrp; /*物理机所在的组个数  CJH ADD 2013.07.25*/
	int at_pmlist_id; //在全局pmlist中的下标
	/*所在框信息*/
    framePtr at_p_fr;   /*物理机所在的框  CJH ADD 2013.07.25*/
	OBJECTID at_fr_sid; /*denotes the frame where PM located， PM对应1个框*/
	
	/*所包含虚拟机信息*/
	vmPtr p_vms[100];/*每个PM最多能容纳100个VM*/
	int nearPM[PM_TOTAL];
	int nearPM_total;
	int countofvm;/*每个PM中包含的VM数*/

	int noNeedJudgeHot;
	int notColdPM;
	int canBePlace;
	/*PM COST预留字段*/
	double performance;
	double cost;
	
	//int rack_id;  /*denotes which rack the PM belongs to*/
} st_pm;

typedef st_pm* pmPtr;/*指向st_pm的指针*/


/*--------------------------------------------------------*/
/*VM traffic rate and affinity property*/
typedef struct{

	int id;
	
	/*存在亲和性的vmid的集合，是个字符串，中间用逗号隔开，当affinity为-1，-2，-3,0 时，内容为空*/
	char vmids[MAX_VM_COUNT_PER_PM];
	
	vmPtr p_vms[MAX_VM_COUNT_PER_PM];

	/*VM个数，默认为2*/
	int countofvm;
	
	/*指向VM对的指针 当affinity为1,2,3时分别表示同板，同框，同组亲和性，
	涉及多个VM，该字段为空*/
	vmPtr p_src_vm,p_dst_vm;

	int groupid;
	//用于记录同组同框正向表的时候 组id


	/*流量大小，当affinity为0时，该值不为空，其它情况为空。*/
	double traffic;


	/*aff_type表示亲和性类型,包括7个值：0~4。
				“0”-表示一般流量关系，不考虑亲和性；
				“+-1”-表示同或不同物理板属性，两个VM必须部署在同一个PM上，affinity_at属性说明放置的具体PM；
				“+-2”-表示同框不同框亲和属性，affinity_at为空，表示只要同框即可，若affinity_at不为空，表示要放置的具体框ID；
				“+-3”-表示同组或不同组亲和性，affinity_at为空，表示只要同组即可，若affinity_at不为空，表示要放置的具体组ID。
				-->*/
	enum affinity_type affinity; 
	
	OBJECTID at;/* 表示亲和性所在的PMID，框ID，或组ID*/
	int i_at;
	int affinity_at;/*表示亲和性所在的PMID，框ID，或组ID*/
	
	/*时延约束，预留	*/
	double delay;
	int nextPtr;
} st_vm2vm; 
typedef st_vm2vm* vm2vmPtr;/*指向st_pm的指针*/

/*--------------------------------------------------------*/

/*PM distance*/
typedef struct{
	int id;
	int pm_src_id;
	int pm_dst_id;
	OBJECTID pm_src_sid; 
	OBJECTID pm_dst_sid; 
	int pm_src_port; //range 0~3, optional , corresponding to eth0~eth3
	int pm_dst_port; //range 0~3, optional , corresponding to eth0~eth3
	double delay; //表示时延的距离
	double bandwidth;//表示带宽容量大小
} st_pm2pm;
typedef st_pm2pm* pm2pmPtr;/*指向st_pm的指针*/

/*--------------------------------------------------------*/

/*A struct for the placement of VMs onto PMs*/
typedef struct{
	int id;/*内码*/
	
	vmPtr p_vm; /*20130726 cjh add*/

	pmPtr p_src_pm;/*20130726 cjh add*/
	pmPtr p_dst_pm;/*20130726 cjh add*/


	int vm_id;/*需要放置或迁移的VM id*/
	OBJECTID vm_sid;  /*a vm requires being migrated*/
	
	int pm_src_id;/*如果是迁移，起点PMID*/
	int pm_dst_id;/*如果是迁移，终点PMID*/
	
	OBJECTID pm_src_sid;/*如果是迁移，起点PMID*/ /*the source PM where the VM is located, */
	OBJECTID pm_dst_sid;/*如果是迁移，终点PMID*/	/*the target PM where the VM is migrated*/	

} st_vm2pm;

typedef st_vm2pm* vm2pmPtr;


typedef struct {
	int id;/*最多可以包含24个pmid*/
	int port;/*最多可以包含24个PORTS*/
	int type;/*0-pm端口，1-sw端口*/
}st_swlink;

/*--------------------------------------------------------*/
/*交换机数据结构*/
typedef struct{
	int id;
	OBJECTID sid;
	char name[20];
	int portnum;/*端口数,最多24个端口，范围1-24，当前交换机已经连接过的端口数*/
	
	/*从本交换机向外连接出去的连接*/
	st_swlink links[SW_PORT_NUM];
	int countoflinks;/*已有连接数*/
	
	int level;/*层级*/
}st_switch;

typedef st_switch* swPtr;

/*Relationship of switches*/
/* modified by cjh 07.28 */

typedef struct{
	int id;
	
	/*CJH ADD 07.28 2013*/
	swPtr p_src_sw;
	int linkport_src;/*连接端口*/
	swPtr p_dst_sw;
	int linkport_dst;/*连接端口*/

	int sw_src_id;
	int sw_dst_id;

	float delay;/*距离*/
	float bandwidth;

	OBJECTID sw_src_sid; 
	OBJECTID sw_dst_sid; 
}st_sw2sw;
typedef st_sw2sw* sw2swPtr;

/*--------------------------------------------------------*/
/*Relationship of pm and switch*/
/* modified by cjh 07.28 */
typedef struct{
	int id;
	
	/*pm端*/
	int pm_id;
	int port_id;/*0-4*/
	OBJECTID pm_sid;
	pmPtr p_pm;

	float delay;/*距离*/
	float bandwidth;

	/*switch端*/
	int switch_id;
	int swithch_port;/*共24个口0-23*/
	OBJECTID switch_sid; 
	swPtr p_sw;

}st_pm2sw;
typedef st_pm2sw* pm2swPtr;
/******************************************************************************/


/******************************************************************************/


/*Action list*/
/*	<?xml version="1.0" encoding="UTF-8"?>
<ACTION VERSION="0.1">
	<MIGRATE ID="ID_1" SRC="10.214.6.111" VMID="vm_1" DST="10.214.16.205"/>
	<MIGRATE ID="ID_2" SRC="10.214.15.111" VMID="vm_2" DST="10.214.16.205"/>
	<MIGRATE ID="ID_3" SRC="10.214.15.102" VMID="vm_3" DST="10.214.16.205" RELY="ID_1"/>
	<MIGRATE ID="ID_4" SRC="10.214.15.102" VMID="vm_4" DST="10.214.16.205" RELY="ID_1 ID_3"/>
	<MIGRATE ID="ID_5" SRC="10.214.15.103" VMID="vm_5" DST="10.214.16.204"/>
	<MIGRATE ID="ID_6" SRC="10.214.15.112" VMID="vm_6" DST="10.214.16.201"/>
	<MIGRATE ID="ID_7" SRC="10.214.15.112" VMID="vm_7" DST="10.214.16.201"/>
	<DOWN ID="ID_8" TARGET="10.214.6.103"/>
	<DOWN ID="ID_9" TARGET="10.214.6.112"/>
	<UP ID="ID_10" NUM="3"/>
	<PLACE ID="ID_11" TARGET="10.214.6.102" VMID="vm_8"/>
</ACTION>*/


typedef struct{
	int id;/*操作ID*/
	int action; /*0-PLACE,1-MIGRATE,2-DOWN,3-UP,4-MSG*/
	
	/*处理迁移的操作信息*/
	vmPtr mig_vm;/*将被迁移的VM*/
	pmPtr mig_src;/*迁移源物理机*/
	pmPtr mig_dst;/*迁移目的物理机*/
	
	/****需要关机的物理机信息*******/
	pmPtr down_pm;

	/**无法处理的VM列表
	<msg id=0 cont="" target="vm1,vm2,..." type="place|hotsolve|loadbalance|coldsolve">
	**/
	int unpack_vm_total;
	vmPtr unpackvm[100];
	char mesg[200];/*格式：the sum of unpackedvms' load : cpu = , men =...*/
	int msgtype;/*message type : 0-PLACE, 1-LOADBALANCE, 2-HOTSOLVE, 3-COLDSOLVE, add by sun in 8-15*/
	/*需要初试放置的VM、PM信息*/
	vmPtr place_vm;/*放置VM*/
	pmPtr place_pm;/*放置的PM*/
} st_action;

typedef st_action* actionPtr;

/*--------------------------------------------------------*/

/*VM组，记录ID构成的组，用于基于流量关系的VM分组*/
typedef struct {
	int groupid;
	OBJECTID sid;
	int vm_id_list[MAX_VM_COUNT_PER_GROUP];/*该组所有VM点的集合*/
	int edge_id_list[MAX_EDGE_PER_GROUP];/*该组的边集合,从g_v2vlist[N]数组中对应的边的序号*/
	int vm_total;//该组VM 的个数
	int edge_count; //该组边的个数
	int is_kernel; //该vmg根据亲和性 是至少两个冲突组的放置核心
	double itraffic;//组内流量
	double otraffic;//组外流量
	int at_pmid;/*初试放置决策后获得的pmid*/
	BOOL aff_pm,aff_fr,aff_gr;//指定板框组的标记
	st_metric load[DIM]; /*loaday*, *denote vmCpu, vmMem, vmDiskio, vmFpga.*/	
	int head;
} st_vm_group;

typedef st_vm_group* vmgPtr;


/*--------------------------------------------------------*/
/*VMid边数组*/
typedef struct{
	int id;
	int id_x;//First vm_id
	int id_y;//second vm_id
	double affinity;
} st_vm_edge;
typedef st_vm_edge* vmedgePtr;


/*********************************************************/
/*该结构是之后将要提到的邻接表结构的一个节点。*/
typedef struct{
	int dst;//终点虚拟机ID
	double traffic;//流量大小
}st_adj_list_node;


/*******************************************************************
虚拟机关系的临界表:该结构在分组算法中用于读取虚拟机之间是否存在联系，
在切割算法中还需要从这个结构中得到流量信息并进行相应的修改与合并操作。
********************************************************************/
typedef struct{
	int vm_id;

	/*与该点有边的顶点集合 以及该边表示的流量大小 by Liu Yuanqing*/
	st_adj_list_node adj_vm_id_list[MAX_VM_COUNT_PER_GROUP];
	/*与该顶点有边的VM个数*/
	int adj_vm_id_count;
} st_adj_vmid_list;//邻接表



/*定义组邻接表*/
typedef struct{
	int part_id;
	
	/*与该组有流量的的组的集合 以及该流量的大小 by Liu Yuanqing*/
	st_adj_list_node adj_part_list[MAX_VM_COUNT_PER_GROUP];
	
	/*与该组有流量的的组的个数by Liu Yuanqing*/
	int adj_part_count;

} st_adj_part_list;



typedef struct{
	int id;/* 对于第i个PM，其4个端口的ID分别为 4i,4i+1,4i+2,4i+3；设有n个PM，m个switch，则switch的ID为4n到4n+m-1 */
	int type;/* 0 for port, 1 for switch */
	int list_total;/* 邻接表元素个数 */
	int adj_list[512];/* 邻接表 */
	int at_pmid;/* 如果该设备为port端口，则有所在的物理机ID，对于switch交换机，此值为-1 */
} st_node;


//////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
   char para_name[30];
   char para_type[20];
   double para_value;
   char para_str_val[200];
   double para_double_arr[DIM];
} st_parameter;

	
typedef struct{
	int algType; /*算法类型*/
	char algName[20];/*算法名称*/
	int paracount;/*参数个数*/
	st_parameter para[10];/*参数列表*/
} st_algorithm;


/*调度请求*/
typedef struct{
	int id;/*序号*/
	char type[40];
	st_algorithm alg;
	BOOL enable;
}st_request;
typedef st_request* requestPtr;

/*********************************************************/

/*denote input system data and output result data*/
typedef struct{

	/*物理机分组、分框*/
	pmgPtr pmglist;
	framePtr framelist;
	
	/*网络拓扑结构信息*/
	swPtr swlist;/*交换机列表*/
	sw2swPtr s2slist;/*交换机之间关系列表*/
	pm2swPtr p2slist;/*物理机与交换机关系列表*/
	
	pmPtr pmlist;/*物理机*/
	vmPtr vmlist;/*虚拟机*/
	
	vm2vmPtr v2vlist;/*虚拟机亲和关系*/
	vm2vmPtr afflist;
	vm2vmPtr sg_afflist; //vmg同组同框正向表

	pm2pmPtr p2plist;
	vm2pmPtr v2plist;
	char *shareStrPtr;

	
	vmgPtr vmglist; /*虚拟机亲和分组*/
	
	/*---调度控制要用----------------------------------*/
	requestPtr reqlist;/*调度请求*/
	requestPtr req_pl,req_lb,req_hs,req_cs;/*指向place,balance,hotsolve,coldsolve 请求的指针*/
	actionPtr actionlist;/*调度实施操作*/
	
	/*物理组、框、物理机总数*/
	int fr_total;
	int pm_total;
	int gr_total;

	/*拓扑结构用*/
	int sw_total;/*交换机的总数*/
	int s2s_total;/*交换机与交换机连接总数*/
	int p2s_total;/*物理机与交换机连接总数*/
	
	int vm_total;/*虚拟机总数*/
	int v2v_total;/*虚拟机亲和关系总数*/
	int afflist_total;
	int sg_afflist_total;
	int p2p_total;/*物理机之间时延关系总数*/
	int v2p_total;/*虚拟机放置关系总数*/
	
	int vmg_total;/*虚拟机分组总数*/

	int isFirstPlace;

	int readFailed;
	
	/*---调度控制要用----------------------------------*/
	int action_total;/*调度实施操作总数*/
	int req_total;/*调度请求总数*/

	double cold_limit[DIM];
	double hot_limit[DIM];

	float swdelay[SW_TOTAL][SW_TOTAL];

	double balance;
	int MaxcountOfBalance;
	double timeLimit, tpstart, tpend;//in seconds.

	double singleTimeLimit, singleStart, singleEnd;

	double singleDFScount, DFScount;

	double maxTryTimesForHot; //用于热点消除的单组放置尝试次数 默认为10

	double weight[DIM];

	int sysNonEmptyPM;

	int lastPlacePM;

} st_share_data;
typedef st_share_data* sharedataPtr;



/**************************************************************/
/**暂时未用，待用数据结构******************/
/**************************************************************/
/*denote tree topology of PM network*/
typedef struct{
	int node_id;
	int port_no;
	int note_type;//PM_NODE 0| SWITCH_NODE 1
} st_treenode;

/*A netlink that links one node to another node*/
typedef struct{
	st_treenode src_node;	
	st_treenode dis_node;
	st_metric band_width;	
} st_netlink;



#endif



