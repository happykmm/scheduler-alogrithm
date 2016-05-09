/*
Module:
This module includes algorithms, with operation functions 
for doing grouping vms and cutting large groups into small 
vm groups which can be placed onto one PMs.
Created by Jianhai Chen, 05-06-2013
*/

#include "cls_vmgroup.h"
#include <string.h>

#include "../scheduler/alg_initplace.h"

//struct affinity_edge{
//	float affinity;
//	vector<int> edge;/*边的集合*/	
//}ed[NN] //存储边数组按照哈希下标来表示的数组
//,g_edge[NN],//保存的是来自文件中的边的集合
//ed_group[NN];//存储重新分组时用的边集合
//
void init_vmgroup(cls_vmgroupPtr THIS){
	
	_gcthis=THIS; /*初始化内部对象指针*/

	THIS->generate_VM_group=generate_VM_group;
	THIS->group_cut=group_cut;
	THIS->output_parts=output_parts;
	THIS->output_actionlist=output_actionlist;
	//THIS->check_aff=check_aff;
	THIS->grouping_in_pm = grouping_in_pm;
	THIS->getVMidstrByvmg=getVMidstrByvmg;
}

void add_new_aff(vmPtr p_vm1,vmPtr p_vm2,enum affinity_type type)
{
	vm2vmPtr afflist;
	int afflist_total;
	afflist = ini_afflist;
	/*处理VM1*/
	afflist_total = ini_afflist_total;
	afflist[afflist_total].affinity = type;
	afflist[afflist_total].countofvm = 2;
	afflist[afflist_total].p_vms[0] = p_vm1;
	afflist[afflist_total].p_vms[1] = p_vm2;
	ini_afflist_total++;
	/*处理VM2*/
	afflist_total = ini_afflist_total;
	afflist[afflist_total].affinity = type;
	afflist[afflist_total].countofvm = 2;
	afflist[afflist_total].p_vms[0] = p_vm2;
	afflist[afflist_total].p_vms[1] = p_vm1;
	ini_afflist_total++;
}

int get_leader(int vm_id,int *vm_leader_list)
{
	if(vm_leader_list[vm_id] < 0) return vm_id;
	vm_leader_list[vm_id] = get_leader(vm_leader_list[vm_id],vm_leader_list);
	return vm_leader_list[vm_id];
}

void merge_set(int leader1,int leader2,int *vm_leader_list)
{
	if(vm_leader_list[leader1] < vm_leader_list[leader2])
	{
		vm_leader_list[leader1] += vm_leader_list[leader2];
		vm_leader_list[leader2] = leader1;
	}
	else
	{
		vm_leader_list[leader2] += vm_leader_list[leader1];
		vm_leader_list[leader1] = leader2;
	}
}

void get_afflist()
{
	int i,j,k;
	int v2v_total,afflist_total;
	int leader1,leader2;
	int *vm_leader_list;
	int *aff_fr_list[FR_TOTAL];
	int aff_fr_list_total = 0;
	int aff_fr_vm_total[FR_TOTAL];
	enum affinity_type type;
	vm2vmPtr v2vlist,p_v2v;
	vm2vmPtr afflist,p_v2v1,p_v2v2;
	vmPtr p_vm1,p_vm2;
	cls_st_frame cls_fr;
	cls_st_pmgroup cls_pmg;
	cls_st_host cls_host;
	initframe(&cls_fr);
	cls_fr.sd = _gcthis->sd;
	initPmgroup(&cls_pmg);
	cls_pmg.sd = _gcthis->sd;
	initHost(&cls_host);
	cls_host.sd = _gcthis->sd;
	v2vlist = _gcthis->sd->v2vlist;
	afflist = ini_afflist;
	v2v_total = _gcthis->sd->v2v_total;
	ini_afflist_total = 0;
	for(i=0;i<_gcthis->sd->vm_total;i++)
	{
		p_vm1 = &_gcthis->sd->vmlist[i];
		p_vm1->aff_pm_head = -1;
		p_vm1->n_aff_pm_head = -1;
		p_vm1->aff_fr_head = -1;
		p_vm1->n_aff_fr_head = -1;
		p_vm1->aff_gr_head = -1;
		p_vm1->n_aff_gr_head = -1;
	}

	/*先处理同框*/
	vm_leader_list = (int *)malloc(sizeof(int) * _gcthis->sd->vm_total);
	for(i=0;i<_gcthis->sd->vm_total;i++)
		vm_leader_list[i] = -1;

	for(i=0;i<v2v_total;i++)
	{
		p_v2v = &v2vlist[i];
		type = p_v2v->affinity;
		if(type != AFF_FR) continue;
		if(p_v2v->countofvm == 1)
		{
			p_vm1 = p_v2v->p_vms[0];
			p_vm1->at_frame_id = cls_fr.getfrbysid(p_v2v->at)->id;
			continue;
		}

		
		for(j=0;j<p_v2v->countofvm-1;j++)
		{
			p_vm1 = p_v2v->p_vms[j];
			for(k=j+1;k<p_v2v->countofvm;k++)
			{
				p_vm2 = p_v2v->p_vms[k];
				/*add_new_aff(p_vm1,p_vm2,type);
				afflist_total = ini_afflist_total;
				afflist[afflist_total-2].nextPtr = p_vm1->aff_fr_head;
				p_vm1->aff_fr_head = afflist_total-2;
				afflist[afflist_total-1].nextPtr = p_vm2->aff_fr_head;
				p_vm2->aff_fr_head = afflist_total-1;*/
				leader1 = get_leader(p_vm1->host.id,vm_leader_list);
				leader2 = get_leader(p_vm2->host.id,vm_leader_list);
				if(leader1 != leader2)
					merge_set(leader1,leader2,vm_leader_list);
			}

		}
	}
	for(i=0;i<_gcthis->sd->vm_total;i++)
	{
		if(vm_leader_list[i] >= 0)
		{
			leader1 = get_leader(i,vm_leader_list);
			p_vm1 = &_gcthis->sd->vmlist[leader1];
			p_vm2 = &_gcthis->sd->vmlist[i];
			for(j=p_vm1->aff_fr_head;j!=-1;j=afflist[j].nextPtr)
			{
				p_vm1 = afflist[j].p_vms[1];
				add_new_aff(p_vm1,p_vm2,AFF_FR);
				afflist_total = ini_afflist_total;
				afflist[afflist_total-2].nextPtr = p_vm1->aff_fr_head;
				p_vm1->aff_fr_head = afflist_total-2;
				afflist[afflist_total-1].nextPtr = p_vm2->aff_fr_head;
				p_vm2->aff_fr_head = afflist_total-1;
			}
			p_vm1 = &_gcthis->sd->vmlist[leader1];
			add_new_aff(p_vm1,p_vm2,AFF_FR);
			afflist_total = ini_afflist_total;
			afflist[afflist_total-2].nextPtr = p_vm1->aff_fr_head;
			p_vm1->aff_fr_head = afflist_total-2;
			afflist[afflist_total-1].nextPtr = p_vm2->aff_fr_head;
			p_vm2->aff_fr_head = afflist_total-1;
		}
	}
	free(vm_leader_list);

	for(i=0;i<v2v_total;i++)
	{
		p_v2v = &v2vlist[i];
		type = p_v2v->affinity;
		switch(type)
		{
		case NO_AFF:
			break;
		case AFF_PM:
			if(p_v2v->countofvm == 1)
			{
				p_vm1 = p_v2v->p_vms[0];
				p_vm1->at_pmid = cls_host.getpmbysid(p_v2v->at)->host.id;
				continue;
			}
			for(j=0;j<p_v2v->countofvm-1;j++)
			{
				p_vm1 = p_v2v->p_vms[j];
				for(k=j+1;k<p_v2v->countofvm;k++)
				{
					p_vm2 = p_v2v->p_vms[k];
					add_new_aff(p_vm1,p_vm2,type);
					afflist_total = ini_afflist_total;
					afflist[afflist_total-2].nextPtr = p_vm1->aff_pm_head;
					p_vm1->aff_pm_head = afflist_total-2;
					afflist[afflist_total-1].nextPtr = p_vm2->aff_pm_head;
					p_vm2->aff_pm_head = afflist_total-1;
				}
			}
			break;
		case AFF_FR:
			break;
		case AFF_GR:
			if(p_v2v->countofvm == 1)
			{
				p_vm1 = p_v2v->p_vms[0];
				p_vm1->at_pmg_id = cls_pmg.getpmgbysid(p_v2v->at)->id;
				continue;
			}
			for(j=0;j<p_v2v->countofvm-1;j++)
			{
				p_vm1 = p_v2v->p_vms[j];
				for(k=j+1;k<p_v2v->countofvm;k++)
				{
					p_vm2 = p_v2v->p_vms[k];
					add_new_aff(p_vm1,p_vm2,type);
					afflist_total = ini_afflist_total;
					afflist[afflist_total-2].nextPtr = p_vm1->aff_gr_head;
					p_vm1->aff_gr_head = afflist_total-2;
					afflist[afflist_total-1].nextPtr = p_vm2->aff_gr_head;
					p_vm2->aff_gr_head = afflist_total-1;
				}
			}
			break;
		case N_AFF_PM:
			p_vm1 = p_v2v->p_vms[0];
			p_vm2 = p_v2v->p_vms[1];
			add_new_aff(p_vm1,p_vm2,type);
			afflist_total = ini_afflist_total;
			afflist[afflist_total-2].nextPtr = p_vm1->n_aff_pm_head;
			p_vm1->n_aff_pm_head = afflist_total-2;
			afflist[afflist_total-1].nextPtr = p_vm2->n_aff_pm_head;
			p_vm2->n_aff_pm_head = afflist_total-1;
			break;
		case N_AFF_FR:
			p_vm1 = p_v2v->p_vms[0];
			p_vm2 = p_v2v->p_vms[1];
			add_new_aff(p_vm1,p_vm2,type);
			afflist_total = ini_afflist_total;
			afflist[afflist_total-2].nextPtr = p_vm1->n_aff_fr_head;
			p_vm1->n_aff_fr_head = afflist_total-2;
			afflist[afflist_total-1].nextPtr = p_vm2->n_aff_fr_head;
			p_vm2->n_aff_fr_head = afflist_total-1;
			for(j=p_vm1->aff_fr_head;j!=-1;j=afflist[j].nextPtr)
			{
				p_v2v1 = &afflist[j];
				add_new_aff(p_v2v1->p_vms[1],p_vm2,type);
				afflist_total = ini_afflist_total;
				afflist[afflist_total-2].nextPtr = p_v2v1->p_vms[1]->n_aff_fr_head;
				p_v2v1->p_vms[1]->n_aff_fr_head = afflist_total-2;
				afflist[afflist_total-1].nextPtr = p_vm2->n_aff_fr_head;
				p_vm2->n_aff_fr_head = afflist_total-1;
			}
			for(j=p_vm2->aff_fr_head;j!=-1;j=afflist[j].nextPtr)
			{
				p_v2v1 = &afflist[j];
				add_new_aff(p_v2v1->p_vms[1],p_vm1,type);
				afflist_total = ini_afflist_total;
				afflist[afflist_total-2].nextPtr = p_v2v1->p_vms[1]->n_aff_fr_head;
				p_v2v1->p_vms[1]->n_aff_fr_head = afflist_total-2;
				afflist[afflist_total-1].nextPtr = p_vm1->n_aff_fr_head;
				p_vm1->n_aff_fr_head = afflist_total-1;
			}
			for(j=p_vm1->aff_fr_head;j!=-1;j=afflist[j].nextPtr)
			{
				p_v2v1 = &afflist[j];
				for(k=p_vm2->aff_fr_head;k!=-1;k=afflist[k].nextPtr)
				{
					p_v2v2 = &afflist[k];
					add_new_aff(p_v2v1->p_vms[1],p_v2v2->p_vms[1],type);
					afflist_total = ini_afflist_total;
					afflist[afflist_total-2].nextPtr = p_v2v1->p_vms[1]->n_aff_fr_head;
					p_v2v1->p_vms[1]->n_aff_fr_head = afflist_total-2;
					afflist[afflist_total-1].nextPtr = p_v2v2->p_vms[1]->n_aff_fr_head;
					p_v2v2->p_vms[1]->n_aff_fr_head = afflist_total-1;
				}
			}
			break;
		case N_AFF_GR:
			p_vm1 = p_v2v->p_vms[0];
			p_vm2 = p_v2v->p_vms[1];
			add_new_aff(p_vm1,p_vm2,type);
			afflist_total = ini_afflist_total;
			afflist[afflist_total-2].nextPtr = p_vm1->n_aff_gr_head;
			p_vm1->n_aff_gr_head = afflist_total-2;
			afflist[afflist_total-1].nextPtr = p_vm2->n_aff_gr_head;
			p_vm2->n_aff_gr_head = afflist_total-1;
			break;
		default:
			break;
		}
	}
}


/*功能：分组主函数，生成结果在ppart指向的分组数组g_part中
ppart：指向g_part的指针
用法：part_component（g_part）
*/
void generate_VM_group(vmgPtr ppart)
{
    int i,j;
	int vm_id_list[VM_TOTAL];
	int ptr_now;
	int vm_id;
	int vm_total;
	double dimLoad[DIM];
	vmPtr vmlist = _gcthis->sd->vmlist;
	vm2vmPtr afflist = ini_afflist;
	
	vmgPtr p_vg;/*cjh add 08.28*/

    grp_number=0;

	_gcthis->grp_number=0;
	 
	_gcthis->sd->vmg_total=0;

	/*1-初始化邻接表 数据结构 并加载数据----OK*/
	create_adj_vm_list();

	/*初始化分组数据结构*/
	init_part(ppart);//初始化

	for(i=0;i<_gcthis->sd->vm_total;i++)
	{
		vmlist[i].at_pmid = vmlist[i].at_frame_id = vmlist[i].at_pmg_id = -1;
	}

	get_afflist();

	for(i=0;i<_gcthis->sd->vm_total;i++)
	{
		if(vmlist[i].at != -1)
		{
			visit[i] = 1;
			vmlist[i].at_pmid = vmlist[i].at;
			vmlist[i].at_vmgid = -1;
			continue;
		}
		if (visit[i] == 0 && (vmlist[i].at_pmid != -1))
		{
			/*cjh add 08.28*/
			int totvmg;
			totvmg=_gcthis->sd->vmg_total;
			p_vg=&_gcthis->sd->vmglist[totvmg];
			sprintf( p_vg->sid,"vmg%d",totvmg);/*获取vmgid*/

			p_vg->groupid = totvmg;
			p_vg->edge_count = 0;
			p_vg->vm_total = 1;
			p_vg->vm_id_list[0] = i;
			p_vg->aff_pm = (vmlist[i].at_pmid != -1);
			p_vg->aff_fr = (vmlist[i].at_frame_id != -1);
			p_vg->aff_gr = (vmlist[i].at_pmg_id != -1);

			visit[i] = 1;
			for(j=0;j<p_vg->vm_total;j++)
			{
				vm_id = p_vg->vm_id_list[j];
				for(ptr_now = _gcthis->sd->vmlist[vm_id].aff_pm_head;ptr_now != -1;ptr_now = afflist[ptr_now].nextPtr)
				{
					vm_total = _gcthis->sd->vmglist[_gcthis->sd->vmg_total].vm_total;
					_gcthis->sd->vmglist[_gcthis->sd->vmg_total].vm_id_list[vm_total] = afflist[ptr_now].p_vms[1]->host.id;
					visit[afflist[ptr_now].p_vms[1]->host.id] = 1;
					_gcthis->sd->vmglist[_gcthis->sd->vmg_total].vm_total++;
				}
			}
			_gcthis->sd->vmg_total++;
		}
	}

	//分组：通过DFS递归
	for(i=0;i<_gcthis->sd->vm_total;i++)
	{
		vm_id_list[i] = _gcthis->sd->vmlist[i].host.id;
	}
	if (1){

		cls_st_sys sys;
		initSys(&sys);
		sys.sd = _gcthis->sd;
		sys.get_max_vec(dimMax);

	}

    for(i=0;i<_gcthis->sd->vm_total;i++){
        if (visit[i]==1)continue;
		memset(dimLoad,0,sizeof(dimLoad));
		ppart[_gcthis->sd->vmg_total].aff_pm = ppart[_gcthis->sd->vmg_total].aff_fr = ppart[_gcthis->sd->vmg_total].aff_gr = 0;
        dfs(i,_gcthis->sd->vmg_total++,ppart,vm_id_list,dimLoad);

		//内部判断是否违反n_aff
		
			



		//dfs(i,grp_number++,ppart);
    }
	//_gcthis->sd->vmg_total = _gcthis->grp_number;

	//output_parts(_gcthis->sd->vmglist);
	//获取完整的亲和信息
	//get_n_aff_fr_set();
	//get_afflist();
	//检查分组结果中是否有不符合反亲和性的组，若有则做切割
	/*for(i=0;i<_gcthis->sd->vmg_total;i++)
	_gcthis->check_aff(i,ppart);
	_gcthis->sd->vmg_total=_gcthis->grp_number;*/

	for(i=0;i<_gcthis->sd->vmg_total;i++){
		set_edge(ppart,i);/*设置边*/
		set_part_load(&ppart[i]);/*设置组的负载信息*/
		ppart[i].at_pmid=-1;/*未放置时 at_pmid=-1*/
	}

}

/* Stoer-Wagner算法求最小割，以实现对任意物理机都装不下的虚拟机组的切割 */
/*
函数名:group_cut
参数说明：
part_no:待切割的虚拟机组ID
ppart:虚拟机组列表
功能：切割ID为part_no的虚拟机组为2组，并保证这样的两组有最小的流量
返回值：最小割流量
Create by Liu Yuanqing
*/
int father[VM_TOTAL];
int getf(int x){
	if (father[x] == x) return x;
	return father[x] = getf(father[x]);
}
void Union(int a,int b){
	int fa = getf(a), fb = getf(b);
	if (fa==fb) return ;
	father[fb] = fa;
}
double group_cut(int part_no,vmgPtr ppart)
{
	int s,t;
	int i,j;
	int grpnum;
	int vm_id;
	int vm_total;

	double ans = INF,min_cut;
	st_adj_list_node *p,*q,*r;

	grpnum=_gcthis->sd->vmg_total;
	vm_total = ppart[part_no].vm_total;

	get_traffic(part_no);

	for(i=0;i<_gcthis->sd->vmglist[part_no].vm_total;i++)
	{
		in_set[i] = 0;
		father[i] = i;//in_set用于存放分组结果，值为TRUE表示在新分组中,FALSE表示在原分组中
	}
	//memset (sub,FALSE,_gcthis->sd->vm_total * sizeof(sub[0]));

	/*$组内虚拟机个数-1次搜索，找到N-1种可能为最小割的割法，并从这些搜索结果中取出流量最小的一种割法*/
	for (i=1;i<ppart[part_no].vm_total;i++)
	{
		min_cut = search(&s,&t,part_no,vm_total);
		
		vm_total--;
		in_set[t] = 1;
		if(min_cut < ans)//找到较优解
		{
			ans = min_cut;
			for(j=0;j<_gcthis->sd->vmglist[part_no].vm_total;j++)
				if (getf(j)==getf(t))
					sub[j] = 1;
				else sub[j] = 0;
		}
		Union(s,t);
		for (j=0;j<_gcthis->sd->vmglist[part_no].vm_total;j++ )//合并S,T两点，更新流量大小
		{
			if ( in_set[j] == FALSE && j != s )
			{
				p = get_listnode(s,j);
				q = get_listnode(j,s);
				r = get_listnode(t,j);
				if(p == NULL && r != NULL)	{
					//addvtxflow(s,j,r->flow);
					g_adj_vmid_list[s].adj_vm_id_list[g_adj_vmid_list[s].adj_vm_id_count].dst = j;
					g_adj_vmid_list[s].adj_vm_id_list[g_adj_vmid_list[s].adj_vm_id_count].traffic = r->traffic;
					g_adj_vmid_list[s].adj_vm_id_count++;
					//addvtxflow(j,s,r->flow);
					g_adj_vmid_list[j].adj_vm_id_list[g_adj_vmid_list[j].adj_vm_id_count].dst = s;
					g_adj_vmid_list[j].adj_vm_id_list[g_adj_vmid_list[j].adj_vm_id_count].traffic = r->traffic;
					g_adj_vmid_list[j].adj_vm_id_count++;
				}
				else if(p != NULL && r!= NULL){
					p->traffic += r->traffic;
					q->traffic += r->traffic;
				}
			}
		}
	}
	if(ans >= INF) return INF;
	//ppart[grp_number].groupid = grp_number;
	//ppart[grp_number].vm_total = 0;
	
	//_gcthis->grp_number
	ppart[grpnum].groupid = grpnum;
	sprintf(ppart[grpnum].sid,"vmg%d",grpnum);/*cjh add 08.26*/
	ppart[grpnum].vm_total = 0;
	vm_total = ppart[part_no].vm_total;
	
	for(i=0;i<vm_total;i++)//创建新分组并将新组的元素加入列表
	{
		if(sub[i] == TRUE)
		{
			vm_id = ppart[part_no].vm_id_list[i];
			ppart[grpnum].vm_id_list[ppart[grpnum].vm_total++] = vm_id;
			_gcthis->sd->vmlist[vm_id].at_vmgid = grpnum;
			
		}
	}
	ppart[part_no].vm_total = 0;
	for (i = 0;i< vm_total; i++)
	if (sub[i]== FALSE){
		ppart[part_no].vm_id_list[ppart[part_no].vm_total++] = ppart[part_no].vm_id_list[i];
		
	}
	
	/*更新分割后的两个组的边与负载的信息*/

	set_edge(ppart,part_no);
	
	set_part_load(&ppart[part_no]);
	
	set_edge(ppart,grpnum);
	
	set_part_load(&ppart[grpnum]);
	ppart[_gcthis->sd->vmg_total].aff_pm = ppart[_gcthis->sd->vmg_total].aff_fr = ppart[_gcthis->sd->vmg_total].aff_gr = 0;

	//初始化at_pmid为-1 Bug in 2013.10.29
	ppart[_gcthis->sd->vmg_total].at_pmid = -1;
	grpnum++;

	_gcthis->sd->vmg_total=grpnum;
	_gcthis->grp_number=grpnum;

	return ans;
}


/*
@功能:创建虚拟机邻接表
@ 过程：读取_gcthis->sd->v2vlist 关系数据；创建邻接表
@ 输出结果：g_adj_vmid_list中；
@调用方法：create_adj_vm_list
@
*/
void create_adj_vm_list()
{
	int i,j;
	int count;//记录边的数量
	int src,dst;
	int grp_1,grp_2;
	int x,y,id;
	enum affinity_type type;
	char vmsid[10][100];
	double traffic;
	for(i=0;i<_gcthis->sd->vm_total;i++)	{
		g_adj_vmid_list[i].vm_id=_gcthis->sd->vmlist[i].host.id;		
		g_adj_vmid_list[i].adj_vm_id_count=0;		
	}
	
	/*从_gcthis->sd->v2vlist中生成邻接虚拟机*/
	count = 0;
	for(i=0;i<_gcthis->sd->v2v_total;i++)
	{
		/*x=_gcthis->sd->v2vlist[j].p_src_vm->host.id;
		y=_gcthis->sd->v2vlist[j].p_dst_vm->host.id;
		traffic=_gcthis->sd->v2vlist[j].traffic;*/
		type = _gcthis->sd->v2vlist[i].affinity;
		switch(type)/*根据类型判断操作*/
		{
		case NO_AFF:/*对于无亲和性的一般流量关系，增加流量信息*/
			x = _gcthis->sd->v2vlist[i].p_vms[0]->host.id;
			y = _gcthis->sd->v2vlist[i].p_vms[1]->host.id;
			traffic=_gcthis->sd->v2vlist[i].traffic;
			//x = _gcthis->sd->v2vlist[i].p_vms[0]->host.id;
			//y = _gcthis->sd->v2vlist[i].p_vms[1]->host.id;
			/*更新边的信息*/
			/*x-->y 的流量信息*/
			id=g_adj_vmid_list[x].adj_vm_id_count;
			g_adj_vmid_list[x].adj_vm_id_list[id].dst=y;
			g_adj_vmid_list[x].adj_vm_id_list[id].traffic=traffic;
			g_adj_vmid_list[x].adj_vm_id_count++;
			/*y-->x 的流量信息*/
			id=g_adj_vmid_list[y].adj_vm_id_count;
			g_adj_vmid_list[y].adj_vm_id_list[id].dst=x;
			g_adj_vmid_list[y].adj_vm_id_list[id].traffic=traffic;
			g_adj_vmid_list[y].adj_vm_id_count++;
			break;
		case AFF_PM:/*对于具有同板亲和性，设置其流量为MAX_TRAFFIC以防止被切割*/
			//for(x=0;x<_gcthis->sd->v2vlist[i].vm_count-1;x++)
				//for(y=x+1;y<_gcthis->sd->v2vlist[i].vm_count;y++)
			for(x=0;x<_gcthis->sd->v2vlist[i].countofvm-1;x++)
				for(y=x+1;y<_gcthis->sd->v2vlist[i].countofvm;y++)
				{
					src = _gcthis->sd->v2vlist[i].p_vms[x]->host.id;
					dst = _gcthis->sd->v2vlist[i].p_vms[y]->host.id;
					for(j=0;j<g_adj_vmid_list[src].adj_vm_id_count;j++)
						if(g_adj_vmid_list[src].adj_vm_id_list[j].dst == dst) break;
					if(j == g_adj_vmid_list[src].adj_vm_id_count)
					{
						g_adj_vmid_list[src].adj_vm_id_count++;
						g_adj_vmid_list[src].adj_vm_id_list[j].dst = dst;
					}
					g_adj_vmid_list[src].adj_vm_id_list[j].traffic = MAX_TRAFFIC;
					for(j=0;j<g_adj_vmid_list[dst].adj_vm_id_count;j++)
						if(g_adj_vmid_list[dst].adj_vm_id_list[j].dst == src) break;
					if(j == g_adj_vmid_list[dst].adj_vm_id_count)
					{
						g_adj_vmid_list[dst].adj_vm_id_count++;
						g_adj_vmid_list[dst].adj_vm_id_list[j].dst = src;
					}
					g_adj_vmid_list[dst].adj_vm_id_list[j].traffic = MAX_TRAFFIC;
				}
		}		
		
		
	}
}


/*初始化part数组,生成邻接表
ppart: 指向全局g_part的指针
用法：init_part(g_part)
*/

/*
函数名:init_part
参数说明：
ppart:虚拟机组列表
功能：初始化虚拟机组
*/
void init_part(vmgPtr ppart)
{
	int i,j;
	
	for(i=0;i<_gcthis->sd->vm_total;i++){
		
		ppart[i].groupid=-1; /*组ID初始化为-1*/
		for(j=0;j<MAX_VM_COUNT_PER_GROUP;j++)
			ppart[i].vm_id_list[j]=-1;//denote null
		
		for(j=0;j<MAX_EDGE_PER_GROUP;j++)
			ppart[i].edge_id_list[j]=-1; //denote null
		
		ppart[i].edge_count=0; /*边数为0*/
		ppart[i].vm_total=0;/*VM数为0*/

		visit[i]=0;//denote this vm is not visited.
	}
}


/*深度优先搜索算法
从邻接表g_adj_vmid_list获取信息生成组
//输入：x-点的号, part_no-分组号
//输出：part数组
用法： dfs(int x,int part_no, g_part)
*/
/*
函数名:dfs
参数说明：
x:当前访问的虚拟机ID
part_no:当前虚拟机组ID
ppart:虚拟机组列表
功能：以深度遍历方式将具有流量关系的一些虚拟机分为一组
*/
void dfs(int x,int part_no, vmgPtr ppart,int vm_id_list[],int dimLoad[])
{
     int i,y;
	 int vm_id = vm_id_list[x];

	 for (i = 0; i<DIM; i++){
		int tmp = _gcthis->sd->vmlist[x].host.load[i].total + dimLoad[i];
		//if (tmp>dimMax[i]) return ; 初始分组不控制大小
		dimLoad[i] = tmp;
	 }
     visit[x]=1;
    // cout<<x<<" ";//<<endl;

	 /*将当前点号加入到当前 part_no对应的组*/
	 ppart[part_no].vm_id_list[ppart[part_no].vm_total++]=vm_id;
	 _gcthis->sd->vmlist[vm_id].at_vmgid = part_no;
	
	 /*对x所在边的每个顶点做搜索与该点关联的边*/
	 for (i=0;i<g_adj_vmid_list[x].adj_vm_id_count;i++){
		 y = g_adj_vmid_list[x].adj_vm_id_list[i].dst;      //当前边
		 
		 if(visit[y]==0 &&  g_adj_vmid_list[x].adj_vm_id_list[i].traffic != 0) 
			 dfs(y,part_no,ppart,vm_id_list,dimLoad);
     }
	 ppart[part_no].groupid=part_no;

	 sprintf(ppart[part_no].sid,"vmg%d",part_no);/*cjh add 08.26*/

}


/*输出分组结果*/
//输入：ppart-指针，指向一个分组part结果的数组part[N]
/*
函数名:output_parts
参数说明：
ppart:虚拟机组列表
功能：输出虚拟机组的信息
*/
void output_parts(vmgPtr ppart)
{
	int i,j,m;
	double used;

	printf("分组个数：%d\n",_gcthis->sd->vmg_total); 

	for(i=0;i<_gcthis->sd->vmg_total;i++){
		printf("分组%3d包括{",i);//,i; 
		for(j=0;j<ppart[i].vm_total;j++)
			printf("%d ",ppart[i].vm_id_list[j]);
		printf("}");
	
		
		printf("对应的边为{");
		for(j=0;j<ppart[i].edge_count;j++)
		{
			m=ppart[i].edge_id_list[j];
			printf("(%d,%d,%4.1f)",_gcthis->sd->v2vlist[m].p_vms[0]->host.id,
				_gcthis->sd->v2vlist[m].p_vms[1]->host.id,
				_gcthis->sd->v2vlist[m].traffic);
		}
				
		printf("}\n");
		
		printf("对应的负载为{");
		for(j=0;j<DIM;j++){
			used=ppart[i].load[j].used;
			printf("(%s,%6.3f)",ppart[i].load[j].metric_name,
				ppart[i].load[j].used);
		}
				
		printf("}\n");

		printf("所在的物理机id为：%d\n",ppart[i].at_pmid);
	}
	
}


void output_actionlist(vmgPtr ppart)
{
	int i,j,k,l,m;
	int place_total,msg_total;
	double used;
	actionPtr p_act;
	
	
	k=0;
	place_total = msg_total = 0;
	
	for(i=0;i<_gcthis->sd->vmg_total;i++){
		for(j=0;j<ppart[i].vm_total;j++){
			p_act=&_gcthis->sd->actionlist[k];
			if(ppart[i].at_pmid != -1)
			{
				p_act->action=0;
				p_act->id=place_total;
				p_act->place_pm=&_gcthis->sd->pmlist[ppart[i].at_pmid];
				p_act->place_vm=&_gcthis->sd->vmlist[ppart[i].vm_id_list[j]];
				place_total++;
			}
			else
			{
				p_act->action=4;
				p_act->id = msg_total;
				p_act->unpack_vm_total = ppart[i].vm_total;
				for(l=0;l<ppart[i].vm_total;l++)
					p_act->unpackvm[l] = &_gcthis->sd->vmlist[ppart[i].vm_id_list[l]];
				strcpy(p_act->mesg,"Some VMs can't be placed");
				msg_total++;
			}
			k++;
		}

	}
	_gcthis->sd->action_total=k;

}


/*假定*/
/*给定一个组号，将其所有的点对应的边的号找出来,加入到part数组*/
/*
函数名:set_edge
参数说明：
ppart:虚拟机组列表
part_no:待设置边的虚拟机组ID
功能：设置虚拟机中的所有边
*/
void set_edge(vmgPtr ppart,int part_no)
{
	int i,j,k,s,e,x,y,xx,yy,flag;
	char vmsid[10][100];
	cls_st_host host;
	initHost(&host);
	host.sd =_gcthis->sd;
	s=ppart[part_no].vm_total;
	ppart[part_no].edge_count = 0;
	for(i=0;i<s; i++)//对每一个点
		for(j=0;j<s;j++){
			if(i<j){
				x=ppart[part_no].vm_id_list[i];			
				y=ppart[part_no].vm_id_list[j];

				for(k=0;k<_gcthis->sd->v2v_total;k++){//E条边进行遍历
					if(_gcthis->sd->v2vlist[k].affinity == NO_AFF)
					{
					//xx=_gcthis->sd->v2vlist[k].p_src_vm->host.id;
					//yy=_gcthis->sd->v2vlist[k].p_dst_vm->host.id;
						xx = _gcthis->sd->v2vlist[k].p_vms[0]->host.id;
						yy = _gcthis->sd->v2vlist[k].p_vms[1]->host.id;
						if((x==xx && y==yy) || (x==yy && y==xx)){
							ppart[part_no].edge_id_list[ppart[part_no].edge_count++]=k;
						}
					}
				}
			}
		}	
}

/*求解亲和组的总负载，为各虚拟机的负载之和*/
/*
函数名:set_part_load
参数说明：
ppart:虚拟机组列表
功能：设置所有虚拟机组的总负载
*/
void set_part_load(vmgPtr ppart)
{
	int i,j,vid;
	st_vm vm;
	for(j=0;j<DIM;j++){
		ppart->load[j].used=ppart->load[j].total=0;
		strcpy(ppart->load[j].metric_name,nameOfMetric[j]);		
	}
	for(i=0;i<ppart->vm_total;i++){
		vid=ppart->vm_id_list[i];
		vm=_gcthis->sd->vmlist[vid];
		for(j=0;j<DIM;j++){
			ppart->load[j].used+=vm.host.load[j].used;
			ppart->load[j].total+=vm.host.load[j].total;
		}
	}	
}

/*获取x到y的距离*/
/*
函数名:get_listnode
参数说明：
x,y:两个虚拟机的ID
功能：获取虚拟机x到虚拟机y的邻接表节点
*/
st_adj_list_node *get_listnode(int x,int y)
{
	int i=0;
	while(i<g_adj_vmid_list[x].adj_vm_id_count 
		&& g_adj_vmid_list[x].adj_vm_id_list[i].dst != y) i++;

	if(i == g_adj_vmid_list[x].adj_vm_id_count) 
		return NULL;
	else 
		return &g_adj_vmid_list[x].adj_vm_id_list[i];
}


/*获取当前组内的流量信息*/
/*
函数名:get_traffic
参数说明：
part_no:当前组ID
功能：生成当前组的邻接表
*/
void get_traffic(int part_no)
{
	int i,j;
	int x,y;
	int src,dst;
	double traffic;
	
	for(i=0;i<_gcthis->sd->vmglist[part_no].vm_total;i++){/*初始化*/
		g_adj_vmid_list[i].adj_vm_id_count = 0;
		g_adj_vmid_list[i].vm_id = _gcthis->sd->vmglist[part_no].vm_id_list[i];
	}


	for(i=0;i<_gcthis->sd->v2v_total;i++){/*遍历所有边*/
		if(_gcthis->sd->v2vlist[i].affinity == NO_AFF)
		{
			x = _gcthis->sd->v2vlist[i].p_vms[0]->host.id;
			y = _gcthis->sd->v2vlist[i].p_vms[1]->host.id;
		
			/*该边两个端点都在这个组里*/
			if(_gcthis->sd->vmlist[x].at_vmgid == part_no 
				&& _gcthis->sd->vmlist[y].at_vmgid == part_no){
				/*将边加入邻接表*/
				traffic = _gcthis->sd->v2vlist[i].traffic;
				
				x = get_id_in_list(x,_gcthis->sd->vmglist[part_no].vm_total,_gcthis->sd->vmglist[part_no].vm_id_list);
				y = get_id_in_list(y,_gcthis->sd->vmglist[part_no].vm_total,_gcthis->sd->vmglist[part_no].vm_id_list);

				g_adj_vmid_list[x].adj_vm_id_list[g_adj_vmid_list[x].adj_vm_id_count].dst = y;
				g_adj_vmid_list[x].adj_vm_id_list[g_adj_vmid_list[x].adj_vm_id_count].traffic = traffic;
				g_adj_vmid_list[x].adj_vm_id_count++;
			
				g_adj_vmid_list[y].adj_vm_id_list[g_adj_vmid_list[y].adj_vm_id_count].dst = x;
				g_adj_vmid_list[y].adj_vm_id_list[g_adj_vmid_list[y].adj_vm_id_count].traffic = traffic;
				g_adj_vmid_list[y].adj_vm_id_count++;
			}
		}
		else if(_gcthis->sd->v2vlist[i].affinity == AFF_PM)
		{
			for(x=0;x<_gcthis->sd->v2vlist[i].countofvm-1;x++)
				for(y=x+1;y<_gcthis->sd->v2vlist[i].countofvm;y++)
				{
					src = _gcthis->sd->v2vlist[i].p_vms[x]->host.id;
					dst = _gcthis->sd->v2vlist[i].p_vms[y]->host.id;
					src = get_id_in_list(src,_gcthis->sd->vmglist[part_no].vm_total,_gcthis->sd->vmglist[part_no].vm_id_list);
					dst = get_id_in_list(dst,_gcthis->sd->vmglist[part_no].vm_total,_gcthis->sd->vmglist[part_no].vm_id_list);
					for(j=0;j<g_adj_vmid_list[src].adj_vm_id_count;j++)
						if(g_adj_vmid_list[src].adj_vm_id_list[j].dst == dst) break;
					if(j == g_adj_vmid_list[src].adj_vm_id_count)
					{
						g_adj_vmid_list[src].adj_vm_id_count++;
						g_adj_vmid_list[src].adj_vm_id_list[j].dst = dst;
					}
					g_adj_vmid_list[src].adj_vm_id_list[j].traffic = MAX_TRAFFIC;
					for(j=0;j<g_adj_vmid_list[dst].adj_vm_id_count;j++)
						if(g_adj_vmid_list[dst].adj_vm_id_list[j].dst == src) break;
					if(j == g_adj_vmid_list[dst].adj_vm_id_count)
					{
						g_adj_vmid_list[dst].adj_vm_id_count++;
						g_adj_vmid_list[dst].adj_vm_id_list[j].dst = src;
					}
					g_adj_vmid_list[dst].adj_vm_id_list[j].traffic = MAX_TRAFFIC;
				}
		}
	}
}

int get_id_in_list(int element,int total, int list[])
{
	int i;
	for(i=0;i<total;i++)
		if(element == list[i]) return i;
	return -1;
}


/*S-W算法中求WAGE并得到S-T最小割的函数*/
/*
函数名:search
参数说明：
ps,pt:用于存放BFS搜索中最后两个点的变量的地址
功能：S-T最小割
返回值：S-T最小割的割值
*/
double search (int *ps,int *pt,int part_no,int vm_total)
{
	double wage[VM_TOTAL];
	double min_cut = 0,mx;
	int u = -1;
	int i,j;
	int dst;
	//flow_p p;
	memset (visit,FALSE,_gcthis->sd->vmglist[part_no].vm_total * sizeof(visit[0]));
	memset (wage,0,_gcthis->sd->vmglist[part_no].vm_total * sizeof(wage[0]));
	*ps = *pt = -1;
	for (i=0;i<vm_total;i++){//从0号虚拟机开始进行BFS搜索
		mx = -1;
		for (j = 0;j<_gcthis->sd->vmglist[part_no].vm_total;j++){
			if (in_set[j] == FALSE && !visit[j] && mx < wage[j])//找到当前WAGE值最大且未访问过的点
			{
				u = j;
				mx = wage[u];
			}
		}
		if ( u == *pt ) return min_cut;
		*ps = *pt; //记录最后找到的两个点
		*pt = u;
		min_cut = mx;
		visit[u] = TRUE;
		j = 0;
		while(j < g_adj_vmid_list[u].adj_vm_id_count){//更新WAGE值

			dst = g_adj_vmid_list[u].adj_vm_id_list[j].dst;
			
			if (!visit[dst]){
				wage[dst] += g_adj_vmid_list[u].adj_vm_id_list[j].traffic;
			}

			j++;
		}
	}
	return min_cut;
}

/****************************************************************************************************/
/*反亲和性检查*/
/*
函数名:check_aff
参数说明：
ppart:虚拟机组列表
part_no:待检查反亲和性的虚拟机组ID
功能：检查虚拟机组中是否有违反反亲和性原则的VM对，
如果有则切开反亲和VM对形成两组
*/

/*void check_aff(int part_no,vmgPtr ppart)
{
	int i;
	int ptr_now;
	int src,dst;
	int src_id,dst_id;
	vm2vmPtr afflist;
	vmPtr p_vm1,p_vm2;
	afflist = _gcthis->sd->afflist;
	for(i=0;i<ppart[part_no].vm_total;i++)
	{
		src = ppart[part_no].vm_id_list[i];
		p_vm1 = &_gcthis->sd->vmlist[src];
		for(ptr_now = p_vm1->n_aff_pm_head;ptr_now!=-1;ptr_now = afflist[ptr_now].nextPtr)
		{
			p_vm2 = afflist[ptr_now].p_vms[1];
			dst = p_vm2->host.id;
			if(p_vm1->at_vmgid == part_no && p_vm2->at_vmgid == part_no)
			{
				src_id = i;
				dst_id = get_id_in_list(dst,ppart[part_no].vm_total,ppart[part_no].vm_id_list);
				group_cut2(part_no,ppart,src_id,dst_id);
			}
		}
		for(ptr_now = p_vm1->n_aff_fr_head;ptr_now!=-1;ptr_now = afflist[ptr_now].nextPtr)
		{
			p_vm2 = afflist[ptr_now].p_vms[1];
			dst = p_vm2->host.id;
			if(p_vm1->at_vmgid == part_no && p_vm2->at_vmgid == part_no)
			{
				src_id = i;
				dst_id = get_id_in_list(dst,ppart[part_no].vm_total,ppart[part_no].vm_id_list);
				group_cut2(part_no,ppart,src_id,dst_id);
			}
		}
		for(ptr_now = p_vm1->n_aff_gr_head;ptr_now!=-1;ptr_now = afflist[ptr_now].nextPtr)
		{
			p_vm2 = afflist[ptr_now].p_vms[1];
			dst = p_vm2->host.id;
			if(p_vm1->at_vmgid == part_no && p_vm2->at_vmgid == part_no)
			{
				src_id = i;
				dst_id = get_id_in_list(dst,ppart[part_no].vm_total,ppart[part_no].vm_id_list);
				group_cut2(part_no,ppart,src_id,dst_id);
			}
		}
	}
}*/
/*void check_aff(int part_no,vmgPtr ppart)
{
	int i,j;
	int aff_total;
	int src,dst,grp1,grp2;
	int src_id,dst_id;
	int x,y;
	int aff;
	cls_st_vm cls_vm;
	initVm(&cls_vm);
	cls_vm.sd = _gcthis->sd;
	aff_total = _gcthis->sd->v2v_total;
	for(i=0;i<ppart[part_no].vm_total;i++)
	{
		src = ppart[part_no].vm_id_list[i];
		grp1 = _gcthis->sd->vmlist[src].at_vmgid;
		for(j=0;j<_gcthis->sd->vmlist[src].n_aff_fr_total;j++)
		{
			dst = _gcthis->sd->vmlist[src].n_aff_fr_list[j];
			grp2 = _gcthis->sd->vmlist[dst].at_vmgid;
			
			if(grp1 == part_no && grp2 == part_no)
			{
				src_id = i;
				dst_id = get_id_in_list(dst,ppart[part_no].vm_total,ppart[part_no].vm_id_list);
				group_cut2(part_no,ppart,src_id,dst_id);
			}
		}
	}
	for(i=0;i<aff_total;i++)
	{
		aff = _gcthis->sd->v2vlist[i].affinity;
		src = _gcthis->sd->v2vlist[i].p_vms[0]->host.id;
		dst = _gcthis->sd->v2vlist[i].p_vms[1]->host.id;
		switch(aff)
		{
		case N_AFF_PM:
		case N_AFF_GR:
			grp1 = _gcthis->sd->vmlist[src].at_vmgid;
			grp2 = _gcthis->sd->vmlist[dst].at_vmgid;
			if(grp1 == part_no && grp2 == part_no)
			{
				src_id = get_id_in_list(src,ppart[part_no].vm_total,ppart[part_no].vm_id_list);
				dst_id = get_id_in_list(dst,ppart[part_no].vm_total,ppart[part_no].vm_id_list);
				group_cut2(part_no,ppart,src_id,dst_id);
			}
			break;
		}
	}
}*/

void get_adj_matric(int part_no,double (*adj_matric)[MAX_VM_COUNT_PER_GROUP])
{
	vmgPtr ppart;
	int i,j;
	int x,y;
	int vm_total;
	int src,dst;
	double traffic;
	
	ppart = _gcthis->sd->vmglist;
	vm_total = ppart[part_no].vm_total;
	for(i=0;i<vm_total;i++)
		for(j=0;j<vm_total;i++)
		{/*初始化*/
			adj_matric[i][j] = -1;
		}


	for(i=0;i<_gcthis->sd->v2v_total;i++){/*遍历所有边*/
		if(_gcthis->sd->v2vlist[i].affinity == NO_AFF)
		{
			x = _gcthis->sd->v2vlist[i].p_vms[0]->host.id;
			y = _gcthis->sd->v2vlist[i].p_vms[1]->host.id;
		
			/*该边两个端点都在这个组里*/
			if(_gcthis->sd->vmlist[x].at_vmgid == part_no 
				&& _gcthis->sd->vmlist[y].at_vmgid == part_no){
				/*将边加入邻接表*/
				traffic = _gcthis->sd->v2vlist[i].traffic;
				
				x = get_id_in_list(x,_gcthis->sd->vmglist[part_no].vm_total,_gcthis->sd->vmglist[part_no].vm_id_list);
				y = get_id_in_list(y,_gcthis->sd->vmglist[part_no].vm_total,_gcthis->sd->vmglist[part_no].vm_id_list);

				if(adj_matric[x][y] == -1) adj_matric[x][y] = adj_matric[y][x] = traffic;
			}
		}
		else if(_gcthis->sd->v2vlist[i].affinity == AFF_PM)
		{
			for(x=0;x<_gcthis->sd->v2vlist[i].countofvm-1;x++)
			{
				src = _gcthis->sd->v2vlist[i].p_vms[x]->host.id;
				src = get_id_in_list(src,_gcthis->sd->vmglist[part_no].vm_total,_gcthis->sd->vmglist[part_no].vm_id_list);
				for(y=x+1;y<_gcthis->sd->v2vlist[i].countofvm;y++)
				{
					
					dst = _gcthis->sd->v2vlist[i].p_vms[y]->host.id;
					dst = get_id_in_list(dst,_gcthis->sd->vmglist[part_no].vm_total,_gcthis->sd->vmglist[part_no].vm_id_list);
					adj_matric[src][dst] = adj_matric[dst][src] = MAX_TRAFFIC;
				}
			}
		}
	}
}

double get_max_flow(int part_no,int vm_now,int dst,double flow_now,int visit[])
{
	vmgPtr ppart;
	int vm_total;
	int i;
	double max_flow;
	int dst_now;
	double traffic_now;
	st_adj_list_node *p,*q;
	ppart = _gcthis->sd->vmglist;
	vm_total = ppart[part_no].vm_total;
	visit[vm_now] = 1;
	for(i=0;i<g_adj_vmid_list[vm_now].adj_vm_id_count;i++)
	{
		p = &g_adj_vmid_list[vm_now].adj_vm_id_list[i];
		dst_now = p->dst;
		if(visit[dst_now] == 1) continue;
		traffic_now = p->traffic;
		if(traffic_now == 0) continue;
		if(traffic_now < flow_now)
			flow_now = traffic_now;
		if(dst_now == dst)
		{
			max_flow = flow_now;
		}
		else
		{
			max_flow = get_max_flow(part_no,dst_now,dst,flow_now,visit);
			if(max_flow == 0) continue;
		}
		p->traffic -= max_flow;
		q = get_listnode(dst_now,vm_now);
		q->traffic += max_flow;
		return max_flow;
	}
	return 0;
}

/*
函数名:group_cut2
参数说明：
ppart:虚拟机组列表
part_no:待检查反亲和性的虚拟机组ID
src,dst:必须切开的两个VM的ID
功能：切割ID为part_no的虚拟机组，并保证src和dst分别位于被切后的两个组中，且切后两组有最小流量
返回值：最小割流量
*/

double group_cut2(int part_no,vmgPtr ppart,int src,int dst)
{
	int i,j;
	//int visit[MAX_VM_COUNT_PER_GROUP];
	int vm_id_list[MAX_VM_COUNT_PER_GROUP];
	int head,tail;
	double min_cut = 0;
	double max_flow_now;
	double dimLoad[DIM];
	//double adj_matric[MAX_VM_COUNT_PER_GROUP][MAX_VM_COUNT_PER_GROUP];
	int vm_total;
	vm_total = ppart[part_no].vm_total;
	//get_adj_matric(part_no,adj_matric);
	get_traffic(part_no);
	memset(visit,0,sizeof(int)*vm_total);
	max_flow_now = get_max_flow(part_no,src,dst,MAX_TRAFFIC,visit);
	while(max_flow_now != 0)
	{
		min_cut += max_flow_now;
		memset(visit,0,sizeof(int)*vm_total);
		max_flow_now = get_max_flow(part_no,src,dst,MAX_TRAFFIC,visit);
	}
	memcpy(vm_id_list,ppart[part_no].vm_id_list,ppart[part_no].vm_total*sizeof(int));

	for(j=0;j<ppart[part_no].vm_total;j++)
	{
		visit[j] = FALSE;
	}
	ppart[part_no].vm_total = 0;
	if (1){

		cls_st_sys sys;
		initSys(&sys);
		sys.sd = _gcthis->sd;
		sys.get_max_vec(dimMax);

	}
	for (i = 0;i< DIM;i ++)
		dimLoad[i] = -INT_MAX;
	dfs(src,part_no,ppart,vm_id_list,dimLoad);

	for (i = 0;i< DIM;i ++)
		dimLoad[i] = -INT_MAX;
	ppart[_gcthis->sd->vmg_total].vm_total = 0;
	dfs(dst,_gcthis->sd->vmg_total,ppart,vm_id_list,dimLoad);

	set_edge(ppart,part_no);
	
	set_part_load(&ppart[part_no]);
	
	set_edge(ppart,_gcthis->sd->vmg_total);
	
	set_part_load(&ppart[_gcthis->sd->vmg_total]);
	ppart[_gcthis->sd->vmg_total].aff_pm = ppart[_gcthis->sd->vmg_total].aff_fr = ppart[_gcthis->sd->vmg_total].aff_gr = 0;
	ppart[_gcthis->sd->vmg_total].at_pmid = -1;
	_gcthis->sd->vmg_total++;
	_gcthis->grp_number = _gcthis->sd->vmg_total;
	return min_cut;
}
/*double group_cut2(int part_no,vmgPtr ppart,int src,int dst)
{
	int i,j;
	int next;
	BOOL cut_ok = FALSE;
	double min_flow,min_cut=0;
	double check_result,flow;
	int vm_id_list[MAX_VM_COUNT_PER_GROUP];
	get_traffic(part_no);
	i = 0;
	do
	{
		min_flow = UNCONNECTED;

		while(i<g_adj_vmid_list[src].adj_vm_id_count)
		{
			for(j=0;j<_gcthis->sd->vm_total;j++)
			{
				visit[j] = FALSE;
			}
			visit[src] = TRUE;
			next = g_adj_vmid_list[src].adj_vm_id_list[i].dst;			
			flow = g_adj_vmid_list[src].adj_vm_id_list[i].traffic;
			if(next == dst)
			{
				min_flow = flow;
				min_cut += flow;
				delete_edge(src,dst);
				break;
			}
			check_result = dfs_check(next,dst,src,next,flow);
			min_flow = MIN(min_flow,check_result);
			if(check_result != UNCONNECTED)
			{
				min_cut += check_result;
			}
			else i++;
			
		}
	}
	while(min_flow != UNCONNECTED);

	memcpy(vm_id_list,ppart[part_no].vm_id_list,ppart[part_no].vm_total*sizeof(int));

	for(j=0;j<ppart[part_no].vm_total;j++)
	{
		visit[j] = FALSE;
	}
	ppart[part_no].vm_total = 0;
	dfs(src,part_no,ppart,vm_id_list);

	ppart[_gcthis->sd->vmg_total].vm_total = 0;
	dfs(dst,_gcthis->sd->vmg_total,ppart,vm_id_list);
	_gcthis->sd->vmg_total++;
	_gcthis->grp_number = _gcthis->sd->vmg_total;
	return min_cut;
}*/

/*
函数名:dfs_check
参数说明：
x:检索到的当前虚拟机
dst:目标虚拟机
min_src,min_dst:检索过程经过的路径中最小的流量边对应的起止点
flow：最小流量边的流量
功能：以深度遍历方式检索x到dst是否有路径，若有则切割该路径上最小的流量边
返回值：有路径则返回被切割的边的流量，否则返回UNCONNECTED
*/
double dfs_check(int x,int dst,int min_src,int min_dst,double flow)
{
	double min_flow;
	double flow_now;
	int i;
	int y;
	int min_src_now,min_dst_now;
	visit[x] = TRUE;
	for(i=0;i<g_adj_vmid_list[x].adj_vm_id_count;i++)
	{
		y = g_adj_vmid_list[x].adj_vm_id_list[i].dst;
		if(visit[y] == FALSE)
		{
			flow_now = g_adj_vmid_list[x].adj_vm_id_list[i].traffic;
			if(flow_now < flow)
			{
				min_src_now = x;
				min_dst_now = y;
			}
			else
			{
				min_src_now = min_src;
				min_dst_now = min_dst;
				flow_now = flow;
			}
			if(y == dst)
			{
				delete_edge(min_src_now,min_dst_now);
				return flow_now;
			}
			min_flow = dfs_check(y,dst,min_src_now,min_dst_now,flow_now);
			if(min_flow != UNCONNECTED) return min_flow;
		}
	}
	return UNCONNECTED;
}

/*
函数名:delete_edge
参数说明：
src,dst:要切割的流量边的起止点VM的ID
功能：将src到dst的流量关系从邻接表中移除
*/
void delete_edge(int src,int dst)
{
	int i,total;
	i=0;
	while(g_adj_vmid_list[src].adj_vm_id_list[i].dst != dst) i++;
	total = --g_adj_vmid_list[src].adj_vm_id_count;
	g_adj_vmid_list[src].adj_vm_id_list[i] = g_adj_vmid_list[src].adj_vm_id_list[total];

	i=0;
	while(g_adj_vmid_list[dst].adj_vm_id_list[i].dst != src) i++;
	total = --g_adj_vmid_list[dst].adj_vm_id_count;
	g_adj_vmid_list[dst].adj_vm_id_list[i] = g_adj_vmid_list[dst].adj_vm_id_list[total];
}

/**************************************/
void create_adj_vm_list_in_pm(int pm_id)
{
	int i,j;
	int count;//记录边的数量
	int src,dst;
	int grp_1,grp_2;
	int x,y,id;
	enum affinity_type type;
	char vmsid[10][100];
	double traffic;
	int vm_total;
	vmPtr *p_vms;
	int pm1_id,pm2_id;
	vm_total = _gcthis->sd->pmlist[pm_id].countofvm;
	p_vms = _gcthis->sd->pmlist[pm_id].p_vms;
	for(i=0;i<vm_total;i++)	{
		g_adj_vmid_list[i].vm_id=p_vms[i]->host.id;		
		g_adj_vmid_list[i].adj_vm_id_count=0;		
	}
	
	/*从_gcthis->sd->v2vlist中生成邻接虚拟机*/
	//count = 0;
	for(i=0;i<_gcthis->sd->v2v_total;i++)
	{
		/*x=_gcthis->sd->v2vlist[j].p_src_vm->host.id;
		y=_gcthis->sd->v2vlist[j].p_dst_vm->host.id;
		traffic=_gcthis->sd->v2vlist[j].traffic;*/
		type = _gcthis->sd->v2vlist[i].affinity;
		if (type<0) continue;
		if (_gcthis->sd->v2vlist[i].countofvm<2)
			continue;
		x = _gcthis->sd->v2vlist[i].p_vms[0]->host.id;
		y = _gcthis->sd->v2vlist[i].p_vms[1]->host.id;

		//pm1_id = _gcthis->sd->vmlist[x].at_pmid;
		//pm2_id = _gcthis->sd->vmlist[y].at_pmid;
		pm1_id = _gcthis->sd->vmlist[x].at;//at_pmid改为at，by sun in 8-15
		pm2_id = _gcthis->sd->vmlist[y].at;

		if(pm1_id != pm2_id || pm1_id != pm_id) break;
		traffic=_gcthis->sd->v2vlist[i].traffic;
		x = get_id_in_p_vms(x,vm_total,p_vms);
		y = get_id_in_p_vms(y,vm_total,p_vms);
		//x = _gcthis->sd->v2vlist[i].p_vms[0]->host.id;
		//y = _gcthis->sd->v2vlist[i].p_vms[1]->host.id;

		/*x-->y 的流量信息*/
		id=g_adj_vmid_list[x].adj_vm_id_count;
		g_adj_vmid_list[x].adj_vm_id_list[id].dst=y;
		g_adj_vmid_list[x].adj_vm_id_list[id].traffic=traffic;
		g_adj_vmid_list[x].adj_vm_id_count++;
		/*y-->x 的流量信息*/
		id=g_adj_vmid_list[y].adj_vm_id_count;
		g_adj_vmid_list[y].adj_vm_id_list[id].dst=x;
		g_adj_vmid_list[y].adj_vm_id_list[id].traffic=traffic;
		g_adj_vmid_list[y].adj_vm_id_count++;
		
		
		
	}
}

int get_id_in_p_vms(int vm_id,int vm_total,vmPtr p_vms[])
{
	int i;
	for(i=0;i<vm_total;i++)
	{
		if(vm_id == p_vms[i]->host.id)
			return i;
	}
	return -1;
}

void dfs2(int x,int part_no, vmgPtr ppart,vmPtr p_vms[])
{
     int i,y;
     visit[x]=1;
    // cout<<x<<" ";//<<endl;

	 /*将当前点号加入到当前 part_no对应的组*/
	 ppart[part_no].vm_id_list[ppart[part_no].vm_total++]=p_vms[x]->host.id;
	 p_vms[x]->at_vmgid = part_no;
	
	 /*对x所在边的每个顶点做搜索与该点关联的边*/
	 for (i=0;i<g_adj_vmid_list[x].adj_vm_id_count;i++){
		 y = g_adj_vmid_list[x].adj_vm_id_list[i].dst;      //当前边
		 
         if(visit[y]==0) dfs2(y,part_no,ppart,p_vms);
     }
	 ppart[part_no].groupid=part_no;
}

void grouping_in_pm(int pm_id)
{
	int i,j;
	cls_st_initplace ini;
	int part_no = _gcthis->sd->vmg_total;
	int pre_vmg_total , vmg_total = 0;
	int vmg_list[VM_TOTAL];
	create_adj_vm_list_in_pm(pm_id);
	for(i=0;i<_gcthis->sd->pmlist[pm_id].countofvm;i++){
		vmPtr vm = _gcthis->sd->pmlist[pm_id].p_vms[i];
		visit[i] = 0;
		for (j=vm->head; j!=-1; j = _gcthis->sd->afflist[j].nextPtr)
		{
			vm2vmPtr aff = &_gcthis->sd->afflist[j];
			//有固定板要求的，不参与DFS分组
			if (aff->affinity==1&&aff->affinity_at==pm_id){

				visit[i] = 1;
				_gcthis->sd->pmlist[pm_id].p_vms[i]->at_pmid = pm_id;
				_gcthis->sd->pmlist[pm_id].p_vms[i]->at_vmgid = -1;
				break;
			}
		}
	}
	
	
	init_initplace(&ini);
	ini.sd=_gcthis->sd;
	
	for(i=0;i<_gcthis->sd->pmlist[pm_id].countofvm;i++)
		if(!visit[i])
		{
			dfs2(i,_gcthis->sd->vmg_total,_gcthis->sd->vmglist ,_gcthis->sd->pmlist[pm_id].p_vms);
			_gcthis->sd->vmg_total ++;
		}
	for (i=part_no;i < _gcthis->sd->vmg_total;i++){
		do{
			pre_vmg_total = vmg_total;
			vmg_total = ini.check_naff(i,_gcthis->sd->vmglist,vmg_total,vmg_list);
		}while(vmg_total!=pre_vmg_total);
	}

}

/**********************************/
/*功能：获取一个组的vmid构成的字符串
 *参数：vmgid 传入的虚拟机组ID， char *ret, 指向返回的字符串数组
 *创建者：陈建海 08.26
*/
void getVMidstrByvmg(int vmgid,char *ret)
{
	int vmid,i,tot;
	vmPtr vm;
	vmgPtr vmg;
	vmg=&_gcthis->sd->vmglist[vmgid];
	tot=vmg->vm_total;
	for(i=0;i<tot;i++)
	{
		vmid=vmg->vm_id_list[i];
		vm=	&_gcthis->sd->vmlist[vmid];
		if(i==0)
			sprintf(ret,"%s",vm->host.sid );
		else
			sprintf(ret,"%s,%s",ret,vm->host.sid );
	}
}
