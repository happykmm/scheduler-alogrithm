/*Definitions of global constant*/
/*全局变量引入文件的定义文件*/
#ifndef _DRS_DRSDEF_H
#define _DRS_DRSDEF_H

/*system state range*/

//#define PM_TOTAL 10000
/* Define system state ranges */


////////////////////////////////////////////////////////
/*store all the input VMs.*/
//extern st_vm g_vmlist[VM_TOTAL]; 
//extern int g_group_vmid_list[VM_TOTAL];/*list all sequeued vmid by grouping*/
//extern st_pm g_pmlist[PM_TOTAL];
//extern float pmdelay[PM_TOTAL][PM_TOTAL];

/*store all the traffic affinity between vms.*/
//extern st_vm2vm g_v2vlist[V2V_TOTAL];
//extern st_pm2pm g_p2plist[P2P_TOTAL];
//extern st_vm2pm g_v2plist[V2P_TOTAL];

/*//////////////////////////////////////////////////////*/
/*共享数据指针的结构*/
//extern st_share_data g_sharedata;


//extern int g_total_vm;
//extern int g_total_pm;
//extern int g_total_vm2vm;
//extern int g_total_v2v;
//extern int g_total_p2p; /*total pm2pm count*/

//对虚拟机进行分组
//extern st_vm_group g_part[VM_TOTAL],part_group[VM_TOTAL];
//extern char nameOfMetric[8][10];

#endif

