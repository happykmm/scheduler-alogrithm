/*
@包含VM、VM2VM数据处理函数的类模块，分组VMG的处理
@创建者：陈建海 2013-06-03
*/

#include "cls_vm.h"
/*
函数名:get_n_aff_fr_set
功能：生成所有虚拟机的不同框虚拟机列表
Create by Liu Yuanqing
*/
/*void get_n_aff_fr_set()
{
	int vm_total,v2v_total,v2vlist_vm_total;
	int i,j,k;
	int set0_total,set1_total;
	int set0[512],set1[512];
	int src,dst;
	int count;
	vmPtr p_vm1,p_vm2;
	vm_total = vthis->sd->vm_total;
	v2v_total = vthis->sd->v2v_total;
	for(i=0;i<vm_total;i++) vthis->sd->vmlist[i].n_aff_fr_total = 0;
	for(i=0;i<v2v_total;i++)
	{
		if(vthis->sd->v2vlist[i].affinity == N_AFF_FR)
		{
			p_vm1 = vthis->sd->v2vlist[i].p_vms[0];
			p_vm2 = vthis->sd->v2vlist[i].p_vms[1];
			
			set0_total = vthis->get_aff_fr_set(p_vm1->host.id,set0);
			set1_total = vthis->get_aff_fr_set(p_vm2->host.id,set1);
			for(j=0;j<set0_total;j++)
			{
				src = set0[j];
				for(k=0;k<set1_total;k++)
				{
					dst = set1[k];
					count = vthis->sd->vmlist[src].n_aff_fr_total;
					vthis->sd->vmlist[src].n_aff_fr_list[count] = dst;
					vthis->sd->vmlist[src].n_aff_fr_total++;
					count = vthis->sd->vmlist[dst].n_aff_fr_total;
					vthis->sd->vmlist[dst].n_aff_fr_list[count] = src;
					vthis->sd->vmlist[dst].n_aff_fr_total++;
				}
			}
			
		}	
	}
}*/

void initVm(clsstvmPtr THIS)
{
	vthis = THIS;
	//THIS->get_n_aff_fr_set = get_n_aff_fr_set;
	//THIS->get_aff_fr_set = get_aff_fr_set;
}

/*
函数名:get_aff_fr_set
参数说明：
vm_id:虚拟机的ID
set:用于存放虚拟机列表的数组
功能：生成ID为vm_id的虚拟机的同框虚拟机列表，放入set数组
Create by Liu Yuanqing
*/
/*int get_aff_fr_set(int vm_id,int set[])
{
	int set_total = 0;
	int vm_total,v2v_total,v2vlist_vm_total;
	BOOL in_set[VM_TOTAL];
	int i,j;
	vm_total = _gcthis->sd->vm_total;
	v2v_total = _gcthis->sd->v2v_total;
	for(i=0;i<vm_total;i++) in_set[i] = FALSE;
	in_set[vm_id] = TRUE;
	for(i=0;i<v2v_total;i++)
	{
		if(_gcthis->sd->v2vlist[i].affinity == AFF_FR)
		{
			v2vlist_vm_total = _gcthis->sd->v2vlist[i].countofvm;
			for(j=0;j<v2vlist_vm_total;j++)
			{
				if(_gcthis->sd->v2vlist[i].p_vms[j]->host.id == vm_id) break;
			}
			if(j<v2vlist_vm_total)
			{
				for(j=0;j<v2vlist_vm_total;j++)
				{
					in_set[_gcthis->sd->v2vlist[i].p_vms[j]->host.id] = TRUE;
				}
			}
		}
	}
	for(i=0;i<vm_total;i++)
	{
		if(in_set[i] == TRUE)
		{
			set[set_total++] = i;
		}
	}
	return set_total;
}*/
