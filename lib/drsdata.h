
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
#define MAX_GR_COUNT_PER_PM 10 /*ÿ��PM��������߼���ĸ���*/
#define MAX_PMCOUNT_PER_FRAME 50/*ÿ����������ɵ����������*/
#define PMIDS_SIZE 50*33 /*��������ID�ַ����ܳ��ȣ�ÿ��PMID���ַ���������10*/
#define PMIDS_SIZE_GR 4000*33 /*��������ID�ַ����ܳ��ȣ�ÿ��PMID���ַ���������10*/
#define MAX_PMCOUNT_PER_GROUP 100/*ÿ����������ɵ����������*/

/******************************************************************/
#define VM_TOTAL 10000  /*VM�ĸ���*/
#define PM_TOTAL 5000 /*pm COUNT , 10000 TO 5000 CJH MODIFY 7.28*/
#define V2V_TOTAL 100000 /*������ϵ�ߵĸ���*/
#define P2P_TOTAL 10000 /*�����֮��ľ���ߵĸ���*/
#define V2P_TOTAL 10000 /*���������������ù�ϵ����*/
#define FR_TOTAL 1000 /*frame count*/
#define PM_GR_TOTAL 5000 /*group count ���������ĸ���*/

/*---��������˹���---------------------*/
#define P2S_TOTAL 16000/*������뽻�������ӵ����� lyq add 7.24*/
#define S2S_TOTAL 10000/*�������뽻�������ӵ����� lyq add 7.24*/
#define SW_TOTAL 450 /*���������� CJH ADD 7.28*//*modified by lyq 8.18*/

/*---���ȿ�����-------------------------*/
#define REQ_TOTAL 100 /*REQUEST TOTAL �������*/
#define ACT_TOTAL 100000 /*����ʵʩ�������� cjh add 7.28*/

#define TREE_NODE_TOTAL 8000 /*pm COUNT about 4000 and switch 4000*/
#define NETLINK_TOTAL 100000 /*netlink COUNT about 10000*/

/*---�����㷨����ȿ�����---------------*/
#define TRUE 1
#define FALSE 0
#define BOOL int
#define MAXDELAY 32767
#define MAX_TRAFFIC 999999 /*��ʾ������С�����ĳ��������ڴ���ͬ���׺���*/
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

/*PM�������ع��*/
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
	//char label[20];			   // ���ڱ�ʾָ������� 
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



/*��������� 2013.07.25CJH*/
typedef struct{
	int id;
	OBJECTID sid;
	char name[40];
	char *pmids;
	int i_pmids[PM_TOTAL];
	int countofpm;//�����ID�ĸ���
} st_pmgroup;
typedef st_pmgroup* pmgPtr;/*ָ��st_pm��ָ��*/



/*�� 2013.07.25CJH*/
typedef struct{
	int id;
	OBJECTID sid;
	char name[30];
	char pmids[PMIDS_SIZE];//�ÿ��µ���������ϣ�������50���壬ÿ�����ID�ַ���������10���ַ�
	int i_pmids[50];/*һ���򲻳���50��PM*/
	int countofpm;//�����ID�ĸ���
} st_frame;
typedef st_frame* framePtr;/*ָ��st_pm��ָ��*/

/*������ 2013.08.09LYQ*/
typedef struct{
	int id;
	int i_pmids[50];/*һ���򲻳���50��PM*/
	int countofpm;//�����ID�ĸ���
	pmgPtr at_p_pmgrps[10]; 
	int countofpmgrp;
	framePtr at_p_fr;
} st_hyper_group;
typedef st_hyper_group* hgPtr;/*ָ��st_pm��ָ��*/


/*--------------------------------------------------------------------------*/

typedef struct{
	int id;   /*Inner single coder*/
	OBJECTID sid; //denote string id or name for VM
	int link_port; /*���ӵĶ˿���*/
	char ip[30];/*�����ⲿ������IP��ַ����ʾΨһ*/
	char eip[PORT_NUM][30]; /*this ip is used for eth*/ 
	int stat; /*����״̬*//*state��-��ʾ�����Է��ã���ʱ��ovs_macΪ��*/
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

	int head; //ָ��afflist������ͷ �±�

	int at; //<!������λ��--> ԭ��VM���õ�λ��

	int at_pmid;//����֮��VM���õ�λ��

	int ask;/*��ʱ���ӣ�������������*/

	//int n_aff_fr_list[300];/*���ڴ�������Ĳ�ͬ���׺�����Ϣ*/
	//int n_aff_fr_total;

	int aff_pm_head,n_aff_pm_head,aff_fr_head,n_aff_fr_head,aff_gr_head,n_aff_gr_head;
	int at_frame_id,at_pmg_id;
} st_vm;

typedef st_vm* vmPtr;

/*--------------------------------------------------------------------------*/
typedef struct{
	st_host host;
	
	/*���ڳ�����*/
	hgPtr at_p_hg;
	/*��������Ϣ*/
	pmgPtr at_p_pmgrps[10]; /*һ��������������10������  CJH ADD 2013.07.25*/
	char at_gr_ids[10*33]; /*denotes which group the PM belongs to��һ��PM���Է����ڶ����*/
	int countofpmgrp; /*��������ڵ������  CJH ADD 2013.07.25*/
	int at_pmlist_id; //��ȫ��pmlist�е��±�
	/*���ڿ���Ϣ*/
    framePtr at_p_fr;   /*��������ڵĿ�  CJH ADD 2013.07.25*/
	OBJECTID at_fr_sid; /*denotes the frame where PM located�� PM��Ӧ1����*/
	
	/*�������������Ϣ*/
	vmPtr p_vms[100];/*ÿ��PM���������100��VM*/
	int nearPM[PM_TOTAL];
	int nearPM_total;
	int countofvm;/*ÿ��PM�а�����VM��*/

	int noNeedJudgeHot;
	int notColdPM;
	int canBePlace;
	/*PM COSTԤ���ֶ�*/
	double performance;
	double cost;
	
	//int rack_id;  /*denotes which rack the PM belongs to*/
} st_pm;

typedef st_pm* pmPtr;/*ָ��st_pm��ָ��*/


/*--------------------------------------------------------*/
/*VM traffic rate and affinity property*/
typedef struct{

	int id;
	
	/*�����׺��Ե�vmid�ļ��ϣ��Ǹ��ַ������м��ö��Ÿ�������affinityΪ-1��-2��-3,0 ʱ������Ϊ��*/
	char vmids[MAX_VM_COUNT_PER_PM];
	
	vmPtr p_vms[MAX_VM_COUNT_PER_PM];

	/*VM������Ĭ��Ϊ2*/
	int countofvm;
	
	/*ָ��VM�Ե�ָ�� ��affinityΪ1,2,3ʱ�ֱ��ʾͬ�壬ͬ��ͬ���׺��ԣ�
	�漰���VM�����ֶ�Ϊ��*/
	vmPtr p_src_vm,p_dst_vm;

	int groupid;
	//���ڼ�¼ͬ��ͬ��������ʱ�� ��id


	/*������С����affinityΪ0ʱ����ֵ��Ϊ�գ��������Ϊ�ա�*/
	double traffic;


	/*aff_type��ʾ�׺�������,����7��ֵ��0~4��
				��0��-��ʾһ��������ϵ���������׺��ԣ�
				��+-1��-��ʾͬ��ͬ��������ԣ�����VM���벿����ͬһ��PM�ϣ�affinity_at����˵�����õľ���PM��
				��+-2��-��ʾͬ��ͬ���׺����ԣ�affinity_atΪ�գ���ʾֻҪͬ�򼴿ɣ���affinity_at��Ϊ�գ���ʾҪ���õľ����ID��
				��+-3��-��ʾͬ���ͬ���׺��ԣ�affinity_atΪ�գ���ʾֻҪͬ�鼴�ɣ���affinity_at��Ϊ�գ���ʾҪ���õľ�����ID��
				-->*/
	enum affinity_type affinity; 
	
	OBJECTID at;/* ��ʾ�׺������ڵ�PMID����ID������ID*/
	int i_at;
	int affinity_at;/*��ʾ�׺������ڵ�PMID����ID������ID*/
	
	/*ʱ��Լ����Ԥ��	*/
	double delay;
	int nextPtr;
} st_vm2vm; 
typedef st_vm2vm* vm2vmPtr;/*ָ��st_pm��ָ��*/

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
	double delay; //��ʾʱ�ӵľ���
	double bandwidth;//��ʾ����������С
} st_pm2pm;
typedef st_pm2pm* pm2pmPtr;/*ָ��st_pm��ָ��*/

/*--------------------------------------------------------*/

/*A struct for the placement of VMs onto PMs*/
typedef struct{
	int id;/*����*/
	
	vmPtr p_vm; /*20130726 cjh add*/

	pmPtr p_src_pm;/*20130726 cjh add*/
	pmPtr p_dst_pm;/*20130726 cjh add*/


	int vm_id;/*��Ҫ���û�Ǩ�Ƶ�VM id*/
	OBJECTID vm_sid;  /*a vm requires being migrated*/
	
	int pm_src_id;/*�����Ǩ�ƣ����PMID*/
	int pm_dst_id;/*�����Ǩ�ƣ��յ�PMID*/
	
	OBJECTID pm_src_sid;/*�����Ǩ�ƣ����PMID*/ /*the source PM where the VM is located, */
	OBJECTID pm_dst_sid;/*�����Ǩ�ƣ��յ�PMID*/	/*the target PM where the VM is migrated*/	

} st_vm2pm;

typedef st_vm2pm* vm2pmPtr;


typedef struct {
	int id;/*�����԰���24��pmid*/
	int port;/*�����԰���24��PORTS*/
	int type;/*0-pm�˿ڣ�1-sw�˿�*/
}st_swlink;

/*--------------------------------------------------------*/
/*���������ݽṹ*/
typedef struct{
	int id;
	OBJECTID sid;
	char name[20];
	int portnum;/*�˿���,���24���˿ڣ���Χ1-24����ǰ�������Ѿ����ӹ��Ķ˿���*/
	
	/*�ӱ��������������ӳ�ȥ������*/
	st_swlink links[SW_PORT_NUM];
	int countoflinks;/*����������*/
	
	int level;/*�㼶*/
}st_switch;

typedef st_switch* swPtr;

/*Relationship of switches*/
/* modified by cjh 07.28 */

typedef struct{
	int id;
	
	/*CJH ADD 07.28 2013*/
	swPtr p_src_sw;
	int linkport_src;/*���Ӷ˿�*/
	swPtr p_dst_sw;
	int linkport_dst;/*���Ӷ˿�*/

	int sw_src_id;
	int sw_dst_id;

	float delay;/*����*/
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
	
	/*pm��*/
	int pm_id;
	int port_id;/*0-4*/
	OBJECTID pm_sid;
	pmPtr p_pm;

	float delay;/*����*/
	float bandwidth;

	/*switch��*/
	int switch_id;
	int swithch_port;/*��24����0-23*/
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
	int id;/*����ID*/
	int action; /*0-PLACE,1-MIGRATE,2-DOWN,3-UP,4-MSG*/
	
	/*����Ǩ�ƵĲ�����Ϣ*/
	vmPtr mig_vm;/*����Ǩ�Ƶ�VM*/
	pmPtr mig_src;/*Ǩ��Դ�����*/
	pmPtr mig_dst;/*Ǩ��Ŀ�������*/
	
	/****��Ҫ�ػ����������Ϣ*******/
	pmPtr down_pm;

	/**�޷������VM�б�
	<msg id=0 cont="" target="vm1,vm2,..." type="place|hotsolve|loadbalance|coldsolve">
	**/
	int unpack_vm_total;
	vmPtr unpackvm[100];
	char mesg[200];/*��ʽ��the sum of unpackedvms' load : cpu = , men =...*/
	int msgtype;/*message type : 0-PLACE, 1-LOADBALANCE, 2-HOTSOLVE, 3-COLDSOLVE, add by sun in 8-15*/
	/*��Ҫ���Է��õ�VM��PM��Ϣ*/
	vmPtr place_vm;/*����VM*/
	pmPtr place_pm;/*���õ�PM*/
} st_action;

typedef st_action* actionPtr;

/*--------------------------------------------------------*/

/*VM�飬��¼ID���ɵ��飬���ڻ���������ϵ��VM����*/
typedef struct {
	int groupid;
	OBJECTID sid;
	int vm_id_list[MAX_VM_COUNT_PER_GROUP];/*��������VM��ļ���*/
	int edge_id_list[MAX_EDGE_PER_GROUP];/*����ı߼���,��g_v2vlist[N]�����ж�Ӧ�ıߵ����*/
	int vm_total;//����VM �ĸ���
	int edge_count; //����ߵĸ���
	int is_kernel; //��vmg�����׺��� ������������ͻ��ķ��ú���
	double itraffic;//��������
	double otraffic;//��������
	int at_pmid;/*���Է��þ��ߺ��õ�pmid*/
	BOOL aff_pm,aff_fr,aff_gr;//ָ�������ı��
	st_metric load[DIM]; /*loaday*, *denote vmCpu, vmMem, vmDiskio, vmFpga.*/	
	int head;
} st_vm_group;

typedef st_vm_group* vmgPtr;


/*--------------------------------------------------------*/
/*VMid������*/
typedef struct{
	int id;
	int id_x;//First vm_id
	int id_y;//second vm_id
	double affinity;
} st_vm_edge;
typedef st_vm_edge* vmedgePtr;


/*********************************************************/
/*�ýṹ��֮��Ҫ�ᵽ���ڽӱ�ṹ��һ���ڵ㡣*/
typedef struct{
	int dst;//�յ������ID
	double traffic;//������С
}st_adj_list_node;


/*******************************************************************
�������ϵ���ٽ��:�ýṹ�ڷ����㷨�����ڶ�ȡ�����֮���Ƿ������ϵ��
���и��㷨�л���Ҫ������ṹ�еõ�������Ϣ��������Ӧ���޸���ϲ�������
********************************************************************/
typedef struct{
	int vm_id;

	/*��õ��бߵĶ��㼯�� �Լ��ñ߱�ʾ��������С by Liu Yuanqing*/
	st_adj_list_node adj_vm_id_list[MAX_VM_COUNT_PER_GROUP];
	/*��ö����бߵ�VM����*/
	int adj_vm_id_count;
} st_adj_vmid_list;//�ڽӱ�



/*�������ڽӱ�*/
typedef struct{
	int part_id;
	
	/*������������ĵ���ļ��� �Լ��������Ĵ�С by Liu Yuanqing*/
	st_adj_list_node adj_part_list[MAX_VM_COUNT_PER_GROUP];
	
	/*������������ĵ���ĸ���by Liu Yuanqing*/
	int adj_part_count;

} st_adj_part_list;



typedef struct{
	int id;/* ���ڵ�i��PM����4���˿ڵ�ID�ֱ�Ϊ 4i,4i+1,4i+2,4i+3������n��PM��m��switch����switch��IDΪ4n��4n+m-1 */
	int type;/* 0 for port, 1 for switch */
	int list_total;/* �ڽӱ�Ԫ�ظ��� */
	int adj_list[512];/* �ڽӱ� */
	int at_pmid;/* ������豸Ϊport�˿ڣ��������ڵ������ID������switch����������ֵΪ-1 */
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
	int algType; /*�㷨����*/
	char algName[20];/*�㷨����*/
	int paracount;/*��������*/
	st_parameter para[10];/*�����б�*/
} st_algorithm;


/*��������*/
typedef struct{
	int id;/*���*/
	char type[40];
	st_algorithm alg;
	BOOL enable;
}st_request;
typedef st_request* requestPtr;

/*********************************************************/

/*denote input system data and output result data*/
typedef struct{

	/*��������顢�ֿ�*/
	pmgPtr pmglist;
	framePtr framelist;
	
	/*�������˽ṹ��Ϣ*/
	swPtr swlist;/*�������б�*/
	sw2swPtr s2slist;/*������֮���ϵ�б�*/
	pm2swPtr p2slist;/*������뽻������ϵ�б�*/
	
	pmPtr pmlist;/*�����*/
	vmPtr vmlist;/*�����*/
	
	vm2vmPtr v2vlist;/*������׺͹�ϵ*/
	vm2vmPtr afflist;
	vm2vmPtr sg_afflist; //vmgͬ��ͬ�������

	pm2pmPtr p2plist;
	vm2pmPtr v2plist;
	char *shareStrPtr;

	
	vmgPtr vmglist; /*������׺ͷ���*/
	
	/*---���ȿ���Ҫ��----------------------------------*/
	requestPtr reqlist;/*��������*/
	requestPtr req_pl,req_lb,req_hs,req_cs;/*ָ��place,balance,hotsolve,coldsolve �����ָ��*/
	actionPtr actionlist;/*����ʵʩ����*/
	
	/*�����顢�����������*/
	int fr_total;
	int pm_total;
	int gr_total;

	/*���˽ṹ��*/
	int sw_total;/*������������*/
	int s2s_total;/*�������뽻������������*/
	int p2s_total;/*������뽻������������*/
	
	int vm_total;/*���������*/
	int v2v_total;/*������׺͹�ϵ����*/
	int afflist_total;
	int sg_afflist_total;
	int p2p_total;/*�����֮��ʱ�ӹ�ϵ����*/
	int v2p_total;/*��������ù�ϵ����*/
	
	int vmg_total;/*�������������*/

	int isFirstPlace;

	int readFailed;
	
	/*---���ȿ���Ҫ��----------------------------------*/
	int action_total;/*����ʵʩ��������*/
	int req_total;/*������������*/

	double cold_limit[DIM];
	double hot_limit[DIM];

	float swdelay[SW_TOTAL][SW_TOTAL];

	double balance;
	int MaxcountOfBalance;
	double timeLimit, tpstart, tpend;//in seconds.

	double singleTimeLimit, singleStart, singleEnd;

	double singleDFScount, DFScount;

	double maxTryTimesForHot; //�����ȵ������ĵ�����ó��Դ��� Ĭ��Ϊ10

	double weight[DIM];

	int sysNonEmptyPM;

	int lastPlacePM;

} st_share_data;
typedef st_share_data* sharedataPtr;



/**************************************************************/
/**��ʱδ�ã��������ݽṹ******************/
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



