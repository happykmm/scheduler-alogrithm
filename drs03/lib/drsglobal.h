/**************************************************
@�ļ���drsglobal.h, ȫ�ֱ��������ļ������ļ�ֻ�����س����������һ��
@�����ߣ�Jianhai Chen, 2013-04-10
****************************************************/
#ifndef _DRS_GLOBAL_H
#define _DRS_GLOBAL_H

/*ʱ�Ӿ���delay array*/
//float pmdelay[PM_TOTAL][PM_TOTAL];


char nameOfMetric[DIM][20] ;

/*store all input netlink list*/
st_treenode g_treenode_list[TREE_NODE_TOTAL];
st_netlink g_netlink_list[NETLINK_TOTAL];

/*------------------------------------------------------------*/
/*store all the input VMs.*/

st_vm g_vmlist[VM_TOTAL]; 
st_pm g_pmlist[PM_TOTAL];
st_vm2vm g_v2vlist[V2V_TOTAL];/*store all the traffic affinity between vms.*/
st_vm2vm g_afflist[V2V_TOTAL];
st_vm2vm g_sg_afflist[V2V_TOTAL];//vmgͬ��ͬ�������


st_pm2pm g_p2plist[P2P_TOTAL];/*store �����֮���ʱ�Ӿ���*/

st_switch g_swlist[SW_TOTAL];/*�洢���н�������Ϣ*/
st_pm2sw g_p2slist[P2S_TOTAL];/*�洢��������������Ĺ�ϵ*/
st_sw2sw g_s2slist[S2S_TOTAL];/*�洢������֮��Ĺ�ϵ*/

st_vm2pm g_v2plist[V2P_TOTAL];/*store VM��PM�ķ��ö�Ӧ��ϵ*/
st_request g_reqlist[REQ_TOTAL];/*�������������*/

st_action g_actionlist[PM_TOTAL];/*���߽����ʵʩ����*/
st_frame g_framelist[FR_TOTAL];/*�洢���п������*/
st_pmgroup g_pmglist[PM_GR_TOTAL];/*�洢������������������*/
st_pm templatepmlist[1000];/*�洢ģ��PM*/ 

char shareStr[PM_TOTAL*PM_GR_TOTAL];//������ַ����ڴ��

/*--------------------------------------------*/
//for grouping vm
/*list all sequeued vmid by grouping*/
int g_vmid_group_list[VM_TOTAL];
//����������з���
st_vm_group g_vmglist[VM_TOTAL],part_group[VM_TOTAL];

/*--------------------------------------------*/
st_share_data  g_sharedata; /*����ȫ�����ݽṹ��ȫ��ָ��*/

/*--------------------------------------------*/


#endif
