/*
 ���Է���ԭ�� 
 ����һ��VM��������У�ÿ��VM����һ������Դ������������õ�һ��������С�
 1��ͬ���칹����
 1��ͬ���£�Ŀ����������Դ�����ŵ��µ�ǰ���£�ʹ������������������٣�
 2���칹�£�
   ��1���ɱ�Ŀ�꣺�������������Ƶ�����£�ʹ�����������Դ�ĳɱ����٣�
   ��2������Ŀ�꣺�������������Ƶ�����£������ܹ��ŵ��µ����������
   ��Դ����������ܴﵽ���ؾ��⣻
 
 2������Ҫ��
 1���������������������Դ����CPU��mem������IO������Ҫ����������������
 VM֮�����ͨ�ţ�ʹ�໥ͨ�ŵ�������ܹ����з��á������顣
 2�������֮����������ϵ�ͨ�ž��룺�����Զ���ɾ�����·�����ڵ�ĸ���ȷ����
 �����Ŀ��Ҫ��ͨ�ſ�����С����������������ݣ����е�ʱ����С��ʱ��=������/����
 3������ԭ����������Ƶ��ͨ�ŵ�VM����������������ͬ��PM��Ҫ��ѡȡPMʱʹPM֮���
 ����Ҫ��С��

 4��PM���������ʾΪһ������ÿ���ڵ���PM�򽻻�����
 PM�ϲ�ͬ������˿ڣ����ܱ����ӵ���ͬ�Ľ���������ͬPM֮�������˿ڣ�����һ��·����
 ·��������ָ�꣺·��ʱ�ӣ�������·���Ͻ������ĸ�������Link���ӱ�ʾ���������˿�֮���
 ���ӡ�

 3������㷨��
 1�������㷨
 ����VM����������VM���з��飻
 ����VM֮����׺��ԣ���VM���з��飻
 2�������㷨
 ѡ��VM���ԣ�
    ������ԣ����ѡȡһ��VM�飬���з��ã�
	��VM���ȷ��õĲ��ԣ���VM�鰴��������Ӵ�С����FFD��
	СVM���ȷ��õĲ��ԣ���VM�鰴���������С�������򣬽��з��ã�
 
 ѡ��������Ĳ��ԣ�
   ��̾�����ԣ�����������ľ��룬ѡȡһ�����������������
   �������ԣ�ѡȡ�ܹ��ŵ��µ��������
   ���ȷ���С��������ԣ�Ϊ��ʹ��������������VM�ŵ��£�ѡ������С���ȷ��£�
 
 
 ����ʱ�ӻ���С���������Ĳ��ԣ�
   1�����飻2������FFD��3��������������̾���ѡȡ��4����VM��һ����PM��
 
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

/*��Ԫ������ģ�ǵ��߼���Ԫ512VM���漰300����塣
Ŀǰ������һ����Ԫ��13�����*28����壬
����364�߼��塣��������20��*13��=260�߼��塣*/

//st_pm pm_bin_list[500]; /*��Ϊ�����õ�PM�б�*/
//st_vm vm_list[512];/*�߼������*/
typedef struct{

	sharedataPtr sd; /*ָ��ȫ�����ݹ�������ָ��*/

	vmgPtr part; /*ָ������ָ��*/


	void (*initplace_by_vmgroup)(vmgPtr ppart);

	//void (*initplace)(); /*���Ⱥ���*/
	
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


void init_initplace(cls_st_initplace *THIS); /*��ʼ��*/
void setRequest_initplace(cls_initplacePtr THIS);


/*�������װ����Է����㷨���Ⱥ���*/
void initplace_by_vmgroup(vmgPtr ppart);

int can_place(int part_no,int pm_no,double hot[],vmgPtr ppart);

/*����ָ����part_no����*/
BOOL pack_group(int part_no,double hot[],vmgPtr ppart);


/*���Է��õ�����ں���*/
void initplace();

void compute_itraffic();//�����ڲ�����
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

