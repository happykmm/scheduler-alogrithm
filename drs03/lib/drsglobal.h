/**************************************************
@文件：drsglobal.h, 全局变量定义文件：该文件只被主控程序包含加载一次
@创建者：Jianhai Chen, 2013-04-10
****************************************************/
#ifndef _DRS_GLOBAL_H
#define _DRS_GLOBAL_H

/*时延矩阵：delay array*/
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
st_vm2vm g_sg_afflist[V2V_TOTAL];//vmg同组同框正向表


st_pm2pm g_p2plist[P2P_TOTAL];/*store 物理机之间的时延矩阵*/

st_switch g_swlist[SW_TOTAL];/*存储所有交换机信息*/
st_pm2sw g_p2slist[P2S_TOTAL];/*存储物理机到交换机的关系*/
st_sw2sw g_s2slist[S2S_TOTAL];/*存储交换机之间的关系*/

st_vm2pm g_v2plist[V2P_TOTAL];/*store VM与PM的放置对应关系*/
st_request g_reqlist[REQ_TOTAL];/*接受请求的数组*/

st_action g_actionlist[PM_TOTAL];/*决策结果的实施数组*/
st_frame g_framelist[FR_TOTAL];/*存储所有框的数组*/
st_pmgroup g_pmglist[PM_GR_TOTAL];/*存储所有物理机分组的数组*/
st_pm templatepmlist[1000];/*存储模板PM*/ 

char shareStr[PM_TOTAL*PM_GR_TOTAL];//共享的字符串内存池

/*--------------------------------------------*/
//for grouping vm
/*list all sequeued vmid by grouping*/
int g_vmid_group_list[VM_TOTAL];
//对虚拟机进行分组
st_vm_group g_vmglist[VM_TOTAL],part_group[VM_TOTAL];

/*--------------------------------------------*/
st_share_data  g_sharedata; /*访问全局数据结构的全局指针*/

/*--------------------------------------------*/


#endif
