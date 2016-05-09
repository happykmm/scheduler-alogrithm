/*
  Module: Initplace module
  要求：输入获取负载数据，获取流量数据，获取PM时延矩阵
  输出：放置结果
  Created by chenjianhai.
  Created date: 2013/04/07
*/

#include "alg_initplace.h"
#include <string.h>
#include <time.h>
/*
@函数功能：调度放置算法实现初试放置决策
参数说明：THIS 指向自己的指针
*/

vmgPtr p_vmg[VM_TOTAL];
int n_p_vmg;
int hg_total;
st_hyper_group hglist[FR_TOTAL];

void init_initplace(cls_initplacePtr THIS){
	
	_ithis=THIS;	

	THIS->initplace_by_vmgroup=initplace_by_vmgroup;
	THIS->check_naff = check_naff;

}


/*初始化算法配置参数来自XML11,CJH ADD*/
void setRequest_initplace(cls_initplacePtr THIS)
{
	/*获取配置参数*/
	/*THIS->hot=THIS->sd->req_cs->alg.para[0].para_value;
	THIS->cold=THIS->sd->req_cs->alg.para[1].para_value;
	THIS->warm=THIS->sd->req_cs->alg.para[2].para_value;*/
	//int i;
	//for(i=0;i<DIM;i++)
		//THIS->hot[i] = THIS->sd->req_cs->alg.para[0].para_double_arr[i];

	/*在该函数中已经将hot_limit, cold_limit参数读入sharedData中，
	static void process_XML1_Node(xmlTextReaderPtr reader, 
							  int *reqcount,
							  int *algparacount
							  ) 
	*/
	int i, iPara,j;
	requestPtr p_req;
	char *str_arr[DIM];
	for (j = 0;j<THIS->sd->req_total;j++)
	{
		p_req = &THIS->sd->reqlist[j];
	
		if (strcmp(p_req->type, "PLACE")==0){

			for (iPara = 0 ; iPara< p_req->alg.paracount; iPara++){
				if (strcmp(p_req->alg.para[iPara].para_type,"DOUBLE")==0){
					if (strcmp(p_req->alg.para[iPara].para_name,"timeLimit")==0){
						THIS->sd->timeLimit = strtod(p_req->alg.para[iPara].para_str_val,NULL);
					}
					if (strcmp(p_req->alg.para[iPara].para_name,"singleTimeLimit")==0){
						THIS->sd->singleTimeLimit = strtod(p_req->alg.para[iPara].para_str_val,NULL);

					}
					if (strcmp(p_req->alg.para[iPara].para_name,"singleDFScount")==0){
						THIS->sd->singleDFScount = strtod(p_req->alg.para[iPara].para_str_val,NULL);
					}

				}
				if (strcmp(p_req->alg.para[iPara].para_type,"DOUBLE_ARRAY")!=0)
					continue;
				split_str(p_req->alg.para[iPara].para_str_val,",",str_arr);
				for (i = 0;i<DIM;i++)
					if (strcmp(p_req->alg.para[iPara].para_name,"hot")==0)
						THIS->sd->hot_limit[i] = strtod(str_arr[i],NULL);
					else if (strcmp(p_req->alg.para[iPara].para_name,"cold")==0)
						THIS->sd->cold_limit[i] = strtod(str_arr[i],NULL);
			}
		}
	}
}

/*获取delay矩阵*/
/*
函数名:get_delay
参数说明:
pm_total:pm_list的元素个数
pm_list[]：物理机列表
功能：计算pm_list对应的时延矩阵pmdelay
Create by Liu Yuanqing
*/
/*void get_delay(int pm_total,int pm_list[])
{
	int i,j;
	int src,dst,src_id,dst_id;
	//tot_pm=_ithis->sd->pm_total;

	for(i=0;i<pm_total;i++)
		for(j=0;j<pm_total;j++) _ithis->sd->pmdelay[i][j] = 0;

	for(i=0;i<_ithis->sd->p2p_total;i++){
		src = _ithis->sd->p2plist[i].pm_src_id;
		dst = _ithis->sd->p2plist[i].pm_dst_id;
		src_id = in_list(src,pm_total,pm_list);
		dst_id = in_list(dst,pm_total,pm_list);
		if(src_id != -1 && dst_id != -1)
			_ithis->sd->pmdelay[src_id][dst_id] = _ithis->sd->pmdelay[dst_id][src_id] =(float)_ithis->sd->p2plist[i].delay;
	}
}*/

/*
函数名:init_at_pmid
功能：初始化所有VM所在的PM为-1（表示待初始放置）
Create by Liu Yuanqing
*/
void init_at_pmid()
{
	int i;
	for(i=0;i<_ithis->sd->vm_total;i++) _ithis->sd->vmlist[i].at_pmid = -1;
}


/*
函数名:set_at_pmid
参数说明:
part_no:待设置所放置的PM的虚拟机组的编号
ppart:虚拟机组列表
功能：为编号为part_no的虚拟机组中的所有虚拟机设置所在的PM，该PM为该组所在PM
Create by Liu Yuanqing
*/
void set_at_pmid(int part_no,vmgPtr ppart)
{
	int i;
	int vm_id;
	for(i=0;i<ppart[part_no].vm_total;i++)
	{
		vm_id = ppart[part_no].vm_id_list[i];
		_ithis->sd->vmlist[vm_id].at_pmid = ppart[part_no].at_pmid;
	}
}

/****************************************************/
/*功能：基于组进行初试放置，采用启发式的装箱方法
  ppart: 指向分组数组数据结构的指针：g_part
  创建：2013-05-15 by JianhaiChen
*******************************************************/
double dimMax[DIM];
int sizeTooLarge(vmgPtr vmg){
	int i;
	for (i = 0;i<DIM;i++)
		if (vmg->load[i].total>dimMax[i]*_ithis->sd->hot_limit[i])
			return 1;
	return 0;

}
cls_st_vm_group cls_vmg;
cls_st_sys css;
void initplace_by_vmgroup(vmgPtr ppart)
{
	int i,j;
	int grp_total;
	//int pm_total;
	int v2p_total;
	double hot[DIM];
	BOOL place_ok = 1;
	pmPtr *p_pms;
	//int pm_list[PM_TOTAL];
	st_vm2pm v2plist[1000];

	
	int needCutVmg;
	double cutRes = 0;
	int singleVMG;
	clock_t tpstart, tpend;
	double dosched_time;
	int *order;
	int vmg_list[VM_TOTAL];
	int pre_vmg_total,vmg_total = 1;


	init_vmgroup(&cls_vmg);
	cls_vmg.sd = _ithis->sd;
	initSys(&css);
	css.sd = _ithis->sd;

	//init_at_pmid();
	grp_total = _ithis->sd->vmg_total;// grp_number;

	//计算内部流量
	//compute_itraffic();

	//按照是否有不同组亲和性排序
	//sort_by_traffic();

	//css.get_max_vec(dimMax);

	//判断内部n_aff冲突
	for (i=0;i < _ithis->sd->vmg_total;i++){
		do{
			pre_vmg_total = vmg_total;
			vmg_total = check_naff(i,_ithis->sd->vmglist,vmg_total,vmg_list);
		}while(vmg_total!=pre_vmg_total);

		//判断负载量是否能放得下最大的物理机
		//continue;
		while(cutRes!=INF&&sizeTooLarge(&_ithis->sd->vmglist[i])){
			cutRes = group_cut(i,_ithis->sd->vmglist);

		}
	}
			


	for (i = 0;i <_ithis->sd->vmg_total;i++)
		p_vmg[i] = &_ithis->sd->vmglist[i];
	p_pms = (pmPtr *)malloc(sizeof(pmPtr) * _ithis->sd->pm_total);
	for(i=0;i<_ithis->sd->pm_total;i++)
	{
		p_pms[i] = &_ithis->sd->pmlist[i];
	}
	
	
	initSys(&css);
	css.sd = _ithis->sd;
	_ithis->sd->tpstart = clock();


	do{
		//指针数组会被random_shuffle 应该重新备份
		
		for (i = 0;i <_ithis->sd->vmg_total;i++)
			p_vmg[i] = &_ithis->sd->vmglist[i];
		css.placeVMGtoPM(p_vmg,_ithis->sd->vmg_total,
			p_pms, _ithis->sd->pm_total,
			_ithis->sd->v2plist, &_ithis->sd->v2p_total);
		if (_ithis->sd->v2p_total==-1){
			//放置失败
			order = (int *)malloc(sizeof(int)*_ithis->sd->vmg_total);

			//order数组表示按可用PM数从小到大排序的VMG编号
			css.getMinAvailCountOfVmg(queue_vmg, _ithis->sd->vmg_total, order);

			singleVMG = 0;
			//优先切割经过搜索的vmg_queue
			for (i= 0 ;i < css.maxSearchDepth+1; i++){
				if (_ithis->sd->vmglist[queue_vmg[order[i]]->groupid].vm_total>1){
					cutRes = group_cut(queue_vmg[order[i]]->groupid,_ithis->sd->vmglist);
					if (cutRes!=INF){
						break;
					}
				}else singleVMG ++;
			}
			//都不能切割，切割其他虚拟机组
			if (cutRes==INF){
				for (i= 0 ;i < _ithis->sd->vmg_total; i++){
					if (_ithis->sd->vmglist[i].vm_total>1){
						cutRes = group_cut(i,_ithis->sd->vmglist);
						if (cutRes!=INF){
							break;
						}
					}else singleVMG ++;
				}
			}
			free(order);
			//如果vmgroup全部不能切割了 运行结束
			
		}else break;
		_ithis->sd->tpend=clock();
	
		dosched_time=(_ithis->sd->tpend-_ithis->sd->tpstart)/CLOCKS_PER_SEC;
		if (dosched_time>_ithis->sd->timeLimit) break;
	}while(1);

	return ;


	for(i=0;i<DIM;i++) hot[i] = _ithis->sd->hot_limit[i];
	n_p_vmg = 0;
	for(i=0;i<grp_total;i++){
		/*pm_total = get_available_pm(i,pm_list);
		if(pm_total == 0)
		{
			printf("虚拟机组%d无法装箱，需要新开物理机\n",i);
			ppart[i].at_pmid = -1;
			continue;
		}*/
		//find_min_star(i,pm_total,pm_list); //基于最小时延找出一组VM。

		place_ok &= pack_group(i,_ithis->sd->hot_limit,ppart);
	}
	if(place_ok == FALSE)/*非递归无法放置，则用递归的方法*/
	{
		p_pms = (pmPtr *)malloc(sizeof(pmPtr) * _ithis->sd->pm_total);
		for(i=0;i<_ithis->sd->pm_total;i++)
		{
			p_pms[i] = &_ithis->sd->pmlist[i];
		}
		//placeVMGtoPM(p_vmg,_ithis->sd->vmg_total,p_pms,_ithis->sd->pm_total,v2plist,&v2p_total);
		NP_place(p_vmg,_ithis->sd->vmg_total);//递归放置VMG的总入口
		free(p_pms);
	}
}

/********************************************
@ 功能：判断给定的负载需求是否能放得下ppm
@ 参数：ppm给定的PM，load: 负载需求指针，若为vm,则判断VM是否能放，
若为vmg，则判断vmg是否能放, dim-维度：0到dim-1
@ 创建者：by Jianhai Chen
/********************************************/
int canpack(pmPtr ppm, st_metric *load,int dim)
{
	double remained;
	int j;
	int packed=1;/*初始假定是可以放的1*/
	
	for(j=0;j<dim;j++) 
	{
		/*在j维度，求出物理机ppm的剩余资源*/
		remained =ppm->host.load[j].total- ppm->host.load[j].reserved - ppm->host.load[j].used; 
		
		/*比较，如果放不下，则表示放不进去*/
		if(remained <= load[j].used) packed=0;		
	}
	
	return packed;
}


/*获取物理机的剩余资源*/
/*
函数名:get_remained
参数说明:
ppm:物理机指针
dim：维度
功能：计算ppm指向的物理机第dim维度的剩余资源
返回值：ppm指向的物理机第dim维度的剩余资源
Create by Liu Yuanqing
*/
double get_remained(pmPtr ppm,int dim)
{	double ret;
	ret=ppm->host.load[dim].total - ppm->host.load[dim].reserved - ppm->host.load[dim].used;
	return ret;
}

/*pm\vm\vmg网络资源维度求和
type=1 -VM,vmg, type=2, pm*/
//这个函数可以干掉他~
double get_sum_net(st_metric *pload, int type)
{
	double ret=0;
	int j;
	
	/*维度4-8是网络资源*/
	for(j=4;j<8;j++){
		if(type == T_VM || type == T_VMG) //vm,vmg
			ret += pload[j].used;
		else  //pm
			ret += pload[j].total - pload[j].reserved - pload[j].used;
	}

	return ret;
}

/*计算内部流量*/
/*
函数名:compute_itraffic
功能：计算所有虚拟机组的内部流量，并记录到数组group_traffic中
Create by Liu Yuanqing
*/
void compute_itraffic()
{
	int i = 0, j;
	memset(group_traffic,0,8*100);
	for(i=0;i<_ithis->sd->vmg_total;i++){
	  for(j=0;j<_ithis->sd->vmglist[i].edge_count;j++){
		  group_traffic[i]+=_ithis->sd->v2vlist[_ithis->sd->vmglist[i].edge_id_list[j]].traffic;
	   }
	   _ithis->sd->vmglist[i].itraffic=group_traffic[i];
	}
}


/*按内部流量排序*/
/*
函数名:sort_by_traffic
功能：根据虚拟机组的内部流量按降序对虚拟机组进行排序
Create by Liu Yuanqing
*/
void sort_by_traffic()
{
	int i,j,max,temp;
	for(i=0;i<_ithis->sd->vmg_total;i++)
     traffic_order_group[i]=i;
	for(i=0;i<_ithis->sd->vmg_total;i++){
		max=i;
		for(j=i+1;j<_ithis->sd->vmg_total;j++)
			if(group_traffic[traffic_order_group[j]]>group_traffic[traffic_order_group[max]])
				max=j;
		temp=traffic_order_group[max];
		traffic_order_group[max]=traffic_order_group[i];
	    traffic_order_group[i]=temp;  
	}
}

/*按时延排序*/
/*
函数名:sort_by_delay
参数说明:
src_id:作为源的物理机ID
pm_total:物理机列表pm_list的元素个数
pm_list:物理机列表
功能：根据物理机列表中的物理机到源物理机的时延按升序对物理机排序
Create by Liu Yuanqing
*/
void sort_by_delay(int src_id,int pm_total,int pm_list[])
{
	int i,j,min;
	st_delay temp;
	cls_st_net cls_net;
	st_delay delaylist[PM_TOTAL];

	initNet(&cls_net);
	cls_net.sd = _ithis->sd;
	//get_delay(pm_total,pm_list);
	for(i=0;i<pm_total;i++)
	{
		delaylist[i].dst_pm_id = pm_list[i];
		delaylist[i].delay = cls_net.get_pm_delay(src_id,pm_list[i]);
	}
	for(i=0;i<pm_total;i++){
		min = i;
		for(j=i+1;j<pm_total;j++)
			if(delaylist[j].delay < delaylist[min].delay)
				min = j;
		temp = delaylist[i];
		delaylist[i] = delaylist[min];
		delaylist[min] = temp;
	}
	for(i=0;i<pm_total;i++)
	{
		pm_list[i] = delaylist[i].dst_pm_id;
	}
}

/*获取物理机降维资源*/
/*
函数名:get_pm_source
参数说明:
pm_total:物理机列表pm_list的元素个数
pm_list:物理机列表
功能：以所有物理机在每一维度的剩余量的平均值为标准，
对物理机进行降维（从8维降为1维），以便于全面衡量一个
物理机剩余资源的多少。降维后的资源大小保存在pm_source
数组中。
Create by Liu Yuanqing
*/
void get_pm_source(int pm_total,int pm_list[])
{
	int i,j;
	int pm_id;
	double avg[8];

	for(i=0;i<8;i++) avg[i] = 0;
	for(i=0;i<pm_total;i++) pm_source[i] = 0;
	for(i=0;i<pm_total;i++)
	{
		pm_id = pm_list[i];
		for(j=0;j<8;j++)
			avg[j] += get_remained(&_ithis->sd->pmlist[pm_id],j);
			//pm_source[i]+=get_remained(&_ithis->sd->pmlist[pm_id],j);
	}
	for(i=0;i<8;i++) avg[i] /= pm_total;
	for(i=0;i<pm_total;i++)
	{
		pm_id = pm_list[i];
		for(j=0;j<8;j++)
			pm_source[i] += get_remained(&_ithis->sd->pmlist[pm_id],j) / avg[j];
	}
}




/*给定的vmg的容量，判断给定的一组PM资源是否能够放得下，用PM资源的1.5倍来决策*/
/*
函数名:pm_grouped
参数说明:
vmg_load:虚拟机组的总负载
pm_load:物理机组的总剩余资源
功能：当物理机组每一维度的总剩余量大于虚拟机组
每一维度总负载的1.5倍时，初步判断该组物理机能放
下
返回值：若初步判断能放下返回TRUE，否则返回FALSE
Create by Liu Yuanqing
*/
int pm_grouped(st_load vmg_load,st_load pm_load)
{
	return (pm_load.cpu >= 1.5* vmg_load.cpu 
		&& pm_load.mem >= 1.5* vmg_load.mem 
		&& pm_load.diskio >= 1.5* vmg_load.diskio 
		&& pm_load.fpga >= 1.5* vmg_load.fpga 
		&& pm_load.eth0 >= 1.5* vmg_load.eth0
		&& pm_load.eth1 >= 1.5* vmg_load.eth1
		&& pm_load.eth2 >= 1.5* vmg_load.eth2
		&& pm_load.eth3 >= 1.5* vmg_load.eth3);
}


/*FindMinStar算法寻找适合的物理机组*/
/*
函数名:find_min_star
参数说明:
grp_id:待放置的虚拟机组的ID
pm_total:物理机列表pm_list的元素个数
pm_list:物理机列表
功能：从pm_list中挑选一系列物理机作为放置范围
挑选原则：以最大物理机为源，优先挑选时延小
的物理机，当初步判断这一系列物理机可放下该
组虚拟机时，停止算法。将挑选的物理机按大小
排序后将其ID放入数组pm_group;
返回值：若初步判断能放下返回TRUE，否则返回FALSE
Create by Liu Yuanqing
*/
void find_min_star(int grp_id,int pm_total,int pm_list[])
{
  int i,j,max;
  int i_id,j_id;
  //int present;
  int temp;
  int src_id;
  //pmPtr p_pm;
  

  
  /*total_pm_source.cpu 
	  = total_pm_source.diskio 
	  = total_pm_source.fpga 
	  = total_pm_source.mem 
	  = total_pm_source.eth0
	  = total_pm_source.eth1
	  = total_pm_source.eth2
	  = total_pm_source.eth3 = 0;*/

  get_pm_source(pm_total,pm_list);

  max=0;
  for(j=1;j<pm_total;j++)//寻找最大物理机
  {
     if(pm_source[j]>pm_source[max])
	    max=j;
  }
  src_id = pm_list[max];
  //pm_group[0]=pm_id;
  //pm_num++;
  /*p_pm = &_ithis->sd->pmlist[pm_id];
  total_pm_source.cpu += get_remained(p_pm,CPU);
  total_pm_source.mem += get_remained(p_pm,MEM);
  total_pm_source.diskio += get_remained(p_pm,DISKIO);
  total_pm_source.fpga += get_remained(p_pm,FPGA);
  total_pm_source.eth0 += get_remained(p_pm,ETH0);
  total_pm_source.eth1 += get_remained(p_pm,ETH1);
  total_pm_source.eth2 += get_remained(p_pm,ETH2);
  total_pm_source.eth3 += get_remained(p_pm,ETH3);//以最大物理机作为源

  total_required_source.cpu = _ithis->sd->vmglist[grp_id].load[CPU].total;
  total_required_source.mem = _ithis->sd->vmglist[grp_id].load[MEM].total;
  total_required_source.diskio = _ithis->sd->vmglist[grp_id].load[DISKIO].total;
  total_required_source.fpga = _ithis->sd->vmglist[grp_id].load[FPGA].total;
  total_required_source.eth0 = _ithis->sd->vmglist[grp_id].load[ETH0].total;
  total_required_source.eth1 = _ithis->sd->vmglist[grp_id].load[ETH1].total;
  total_required_source.eth2 = _ithis->sd->vmglist[grp_id].load[ETH2].total;
  total_required_source.eth3 = _ithis->sd->vmglist[grp_id].load[ETH3].total;*/
  
  sort_by_delay(src_id,pm_total,pm_list);
  
  /*while(!pm_grouped(total_required_source,total_pm_source) 
	  && pm_num < pm_total){//以1.5倍虚拟机组负载为标准搜索物理机组
  
	  pm_group[pm_num] = delaylist[pm_num].dst_pm_id;

	  total_pm_source.cpu += get_remained(&_ithis->sd->pmlist[pm_group[pm_num]],CPU);
	  total_pm_source.mem += get_remained(&_ithis->sd->pmlist[pm_group[pm_num]],MEM);
	  total_pm_source.diskio += get_remained(&_ithis->sd->pmlist[pm_group[pm_num]],DISKIO);
	  total_pm_source.fpga += get_remained(&_ithis->sd->pmlist[pm_group[pm_num]],FPGA);

	  total_pm_source.eth0 += get_remained(&_ithis->sd->pmlist[pm_group[pm_num]],ETH0);
	  total_pm_source.eth1 += get_remained(&_ithis->sd->pmlist[pm_group[pm_num]],ETH1);
	  total_pm_source.eth2 += get_remained(&_ithis->sd->pmlist[pm_group[pm_num]],ETH2);
	  total_pm_source.eth3 += get_remained(&_ithis->sd->pmlist[pm_group[pm_num]],ETH3);

	  pm_num++;
  }
	  
  //选择法排序
  for(i=0;i<pm_num;i++){
      max=i;
	  for(j=i+1;j<pm_num;j++)
	  {
		  i_id = in_list(pm_group[max],pm_total,pm_list);
		  j_id = in_list(pm_group[j],pm_total,pm_list);
		  if(pm_source[j_id]>pm_source[i_id])
             max=j;
	  }

	  temp=pm_group[max];
	  pm_group[max]=pm_group[i];
	  pm_group[i]=temp;
  } //对物理机组按大小排序*/
}

/*
函数名:check_naff
参数说明：
ppart:虚拟机组列表
part_no:待检查反亲和性的虚拟机组ID
功能：检查虚拟机组中是否有违反反亲和性原则的VM对，
如果有则切开反亲和VM对形成两组
*/

int check_naff(int part_no,vmgPtr ppart,int vmg_total,int vmg_list[])
{
	int i;
	int ptr_now;
	int src,dst;
	int src_id,dst_id;
	vm2vmPtr afflist;
	vmPtr p_vm1,p_vm2;
	afflist = ini_afflist;
	for(i=0;i<ppart[part_no].vm_total;i++)
	{
		src = ppart[part_no].vm_id_list[i];
		p_vm1 = &_ithis->sd->vmlist[src];
		for(ptr_now = p_vm1->n_aff_pm_head;ptr_now!=-1;ptr_now = afflist[ptr_now].nextPtr)
		{
			p_vm2 = afflist[ptr_now].p_vms[1];
			dst = p_vm2->host.id;
			if(p_vm1->at_vmgid == part_no && p_vm2->at_vmgid == part_no)
			{
				src_id = i;
				dst_id = get_id_in_list(dst,ppart[part_no].vm_total,ppart[part_no].vm_id_list);
				vmg_list[vmg_total++] = _ithis->sd->vmg_total;
				group_cut2(part_no,ppart,src_id,dst_id);
				add_new_p_vmg(&ppart[_ithis->sd->vmg_total-1]);
				return vmg_total;
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
				vmg_list[vmg_total++] = _ithis->sd->vmg_total;
				group_cut2(part_no,ppart,src_id,dst_id);
				add_new_p_vmg(&ppart[_ithis->sd->vmg_total-1]);
				return vmg_total;
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
				vmg_list[vmg_total++] = _ithis->sd->vmg_total;
				group_cut2(part_no,ppart,src_id,dst_id);
				add_new_p_vmg(&ppart[_ithis->sd->vmg_total-1]);
				return vmg_total;
			}
		}
	}
	return vmg_total;
}

int get_max_pm(int pm_total,int pm_list[])
{
	int max;
	int i;
	int src_id;
	get_pm_source(pm_total,pm_list);

	max=0;
	for(i=1;i<pm_total;i++)//寻找最大物理机
	{
		if(pm_source[i]>pm_source[max])
		max=i;
	}
	src_id = pm_list[max];
	return src_id;
}

int can_place(int part_no,int pm_no,double hot[],vmgPtr ppart)
{

	int j;
	double remained,avail;

	/*判断基本维度是否可装*/
	for(j=0;j<8;j++){
		avail = _ithis->sd->pmlist[pm_no].host.load[j].total - _ithis->sd->pmlist[pm_no].host.load[j].reserved;
		if(avail>_ithis->sd->pmlist[pm_no].host.load[j].total * hot[j]) 
			avail = _ithis->sd->pmlist[pm_no].host.load[j].total * hot[j];
		//remained = sh.getTotal(j) * HOT - sh.getUsed(j);/*获取不产生热点的剩余资源大小；修改于8-13，by sun*/
		remained =avail - _ithis->sd->pmlist[pm_no].host.load[j].used;
		if(ppart[part_no].load[j].total > remained) return FALSE;/*判断是否可装*/
	}

	return TRUE;
}

void add_new_p_vmg(vmgPtr vmg)
{
	p_vmg[n_p_vmg] = vmg;
	n_p_vmg++;
}

/*
 * @fn int connectServer(const char *host, int port)
 * 对编号为part_no的组进行装箱决策，策略bestfit
 * @param part_no: 组序号 
 * @param ppart：指针，指向虚拟机流量关系的组 构成的数组
 * @ 无返回
 * @created by jianhai at 2013.5.12
 * @modified by jianhai at 2013.5.21
 */
/*对当前组进行装箱*/
BOOL pack_group(int part_no,double hot[],vmgPtr ppart)
{
	int i,j,k;
	int packed,pm_no;
	double min_cut;
	int pm_total;
	int pm_list[PM_TOTAL];
	int pm_src_id;
	int vmg_list[VM_TOTAL];
	int pre_vmg_total,vmg_total = 1;
	cls_st_sys sys;
	initSys(&sys);
	sys.sd = _ithis->sd;
	//double remained;//剩余量
	vmg_list[0] = part_no;
	add_new_p_vmg(&ppart[part_no]);
	do
	{
		pre_vmg_total = vmg_total;
		vmg_total = check_naff(part_no,ppart,vmg_total,vmg_list);
	}
	while(vmg_total != pre_vmg_total);

	for(i=0;i<vmg_total;i++)
	{
		pm_no = -1;
		while(pm_no == -1){
			pm_total = get_available_pm(vmg_list[i],pm_list);
			if(i == 0)
			{
				pm_src_id = get_max_pm(pm_total,pm_list);
			}
			sort_by_delay(pm_src_id,pm_total,pm_list);
			for(j=0;j<pm_total;j++)//遍历所有物理机
			{
				packed = can_place(vmg_list[i],pm_list[j],hot,ppart);
				if(packed)//如果可装，采用FIRST FIT
				{
					pm_no = pm_list[j];
					break;
				}
			}
			if(pm_no != -1){ //找到某一台物理机可装

				ppart[vmg_list[i]].at_pmid = pm_no;/*设置当前组对应的物理机*/

				for(j=0;j<8;j++){
					_ithis->sd->pmlist[pm_no].host.load[j].used 
						+= ppart[vmg_list[i]].load[j].total;//更新负载
				}
				set_at_pmid(vmg_list[i],ppart);
			}
			else
			{
				if(ppart[vmg_list[i]].vm_total > 1)//组内虚拟机多于1台才可切割
				{
					min_cut = group_cut(vmg_list[i],ppart);//无可装的物理机，则做切割
					if(min_cut < INF)//该组未被同板亲和性限制，可切割
					{
						//pack_group(_ithis->sd->vmg_total-1,ppart);//对切割出的新组进行装箱
						vmg_list[vmg_total++] = _ithis->sd->vmg_total-1;
					}
					else
					{
						printf("虚拟机组%d无法放置，需要新开物理机\n",vmg_list[i]);
						ppart[vmg_list[i]].at_pmid = -1;
						return FALSE;
					}
				}
				else
				{
					printf("虚拟机组%d无法放置，需要新开物理机\n",vmg_list[i]);
					ppart[vmg_list[i]].at_pmid = -1;
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

/*
函数名:get_available_pm
参数说明:
grp_id:待放置的虚拟机组ID
pm_list:可放置的PM列表
功能：根据亲和性与反亲和性计算ID为grp_id的虚拟机组可以放置的物理机范围
返回值：pm_list中的元素个数
Create by Liu Yuanqing
*/

int get_available_pm(int grp_id,int pm_list[])
{
	int in_set[PM_TOTAL],in_set2[PM_TOTAL];
	int count = 0;
	int vm_id,pm_id;
	int ptrnow;
	int *vm_id_list = _ithis->sd->vmglist[grp_id].vm_id_list;
	int vm_total = _ithis->sd->vmglist[grp_id].vm_total;
	int pm_total = _ithis->sd->pm_total;
	pmPtr p_pm;
	pmgPtr p_pmg;
	framePtr p_frame;
	vmPtr p_vm1,p_vm2;
	vm2vmPtr afflist;
	enum affinity_type aff;
	int i,j,k;

	afflist = ini_afflist;
	for(i=0;i<pm_total;i++) in_set[i] = TRUE;
	if(_ithis->sd->vmglist[grp_id].vm_total == 1)
	{
		p_vm1 = &_ithis->sd->vmlist[vm_id_list[0]];
		if(p_vm1->at_pmid != -1)
		{
			for(i=0;i<pm_total;i++)
			{
				if(i != p_vm1->at_pmid) in_set[i] = FALSE;
			}
		}
		if(p_vm1->at_frame_id != -1)
		{
			for(j=0;j<pm_total;j++) in_set2[j] = FALSE;
			p_frame = &_ithis->sd->framelist[p_vm1->at_frame_id];
			for(j=0;j<p_frame->countofpm;j++)
			{
				pm_id = p_frame->i_pmids[j];
				in_set2[pm_id] = TRUE;
			}
			for(j=0;j<pm_total;j++) in_set[j] &= in_set2[j];
		}
		if(p_vm1->at_pmg_id != -1)
		{
			for(j=0;j<pm_total;j++) in_set2[j] = FALSE;
			p_pmg = &_ithis->sd->pmglist[p_vm1->at_pmg_id];
			for(k=0;k<p_pmg->countofpm;k++)
			{
				pm_id = p_pmg->i_pmids[k];
				in_set2[pm_id] = TRUE;
			}
			for(k=0;k<pm_total;k++) in_set[k] &= in_set2[k];
		}
	}
	for(i=0;i<vm_total;i++)
	{
		vm_id = vm_id_list[i];
		p_vm1 = &_ithis->sd->vmlist[vm_id];
		/*不同板处理*/
		for(ptrnow = p_vm1->n_aff_pm_head;ptrnow != -1;ptrnow = afflist[ptrnow].nextPtr)
		{
			p_vm2 = afflist[ptrnow].p_vms[1];
			pm_id = p_vm2->at_pmid;
			if(pm_id != -1) in_set[pm_id] = FALSE;
		}
		/*同框处理*/
		for(ptrnow = p_vm1->aff_fr_head;ptrnow != -1;ptrnow = afflist[ptrnow].nextPtr)
		{
			for(j=0;j<pm_total;j++) in_set2[j] = FALSE;
			p_vm2 = afflist[ptrnow].p_vms[1];
			pm_id = p_vm2->at_pmid;
			if(pm_id != -1)
			{
				p_frame = _ithis->sd->pmlist[pm_id].at_p_fr;
				for(j=0;j<p_frame->countofpm;j++)
				{
					pm_id = p_frame->i_pmids[j];
					in_set2[pm_id] = TRUE;
				}
				for(j=0;j<pm_total;j++) in_set[j] &= in_set2[j];
			}
		}
		/*不同框处理*/
		for(ptrnow = p_vm1->n_aff_fr_head;ptrnow != -1;ptrnow = afflist[ptrnow].nextPtr)
		{
			p_vm2 = afflist[ptrnow].p_vms[1];
			pm_id = p_vm2->at_pmid;
			if(pm_id != -1)
			{
				p_frame = _ithis->sd->pmlist[pm_id].at_p_fr;
				for(j=0;j<p_frame->countofpm;j++)
				{
					pm_id = p_frame->i_pmids[j];
					in_set[pm_id] = FALSE;
				}
			}
		}
		/*同组处理*/
		for(ptrnow = p_vm1->aff_gr_head;ptrnow != -1;ptrnow = afflist[ptrnow].nextPtr)
		{
			for(j=0;j<pm_total;j++) in_set2[j] = FALSE;
			p_vm2 = afflist[ptrnow].p_vms[1];
			pm_id = p_vm2->at_pmid;
			if(pm_id != -1)
			{
				p_pm = &_ithis->sd->pmlist[pm_id];
				for(j=0;j<p_pm->countofpmgrp;j++)
				{
					p_pmg = _ithis->sd->pmlist[pm_id].at_p_pmgrps[j];
					for(k=0;k<p_pmg->countofpm;k++)
					{
						pm_id = p_pmg->i_pmids[k];
						in_set2[pm_id] = TRUE;
					}
					for(k=0;k<pm_total;k++) in_set[k] &= in_set2[k];
				}
			}
		}
		/*不同组处理*/
		for(ptrnow = p_vm1->n_aff_gr_head;ptrnow != -1;ptrnow = afflist[ptrnow].nextPtr)
		{
			p_vm2 = afflist[ptrnow].p_vms[1];
			pm_id = p_vm2->at_pmid;
			if(pm_id != -1)
			{
				p_pm = &_ithis->sd->pmlist[pm_id];
				for(j=0;j<p_pm->countofpmgrp;j++)
				{
					p_pmg = _ithis->sd->pmlist[pm_id].at_p_pmgrps[j];
					for(k=0;k<p_pmg->countofpm;k++)
					{
						pm_id = p_pmg->i_pmids[k];
						in_set[pm_id] = FALSE;
					}
				}
			}
		}
	}
	for(i=0;i<pm_total;i++)
	{
		if(in_set[i] == TRUE)
		{
			pm_list[count++] = i;
		}
	}
	return count;
}

/*int get_available_pm(int grp_id,int pm_list[])
{
	int in_set[PM_TOTAL],in_set2[PM_TOTAL];
	int count = 0;
	int pm_total = _ithis->sd->pm_total;
	int grp_id_2;
	int pm_id;
	int vm_id;
	BOOL mark;
	pmgPtr p_pmg;
	framePtr p_frame;
	vmPtr p_vm1,p_vm2;
	enum affinity_type aff;
	int i,j,k,l,m,n;
	for(i=0;i<pm_total;i++) in_set[i] = TRUE;
	for(i=0;i<_ithis->sd->vmglist[grp_id].vm_total;i++)
	{
		vm_id = _ithis->sd->vmglist[grp_id].vm_id_list[i];
		p_vm1 = &_ithis->sd->vmlist[vm_id];
		for(j=0;j<p_vm1->n_aff_fr_total;j++)
		{
			vm_id = p_vm1->n_aff_fr_list[j];
			pm_id = _ithis->sd->vmlist[vm_id].at_pmid;
			if(pm_id != -1)
			{
				p_frame = _ithis->sd->pmlist[pm_id].at_p_fr;
				for(k=0;k<p_frame->countofpm;k++) in_set[p_frame->i_pmids[k]] = FALSE;
			}
		}
	}
	for(i=0;i<_ithis->sd->v2v_total;i++)
	{
		aff = _ithis->sd->v2vlist[i].affinity;
		if(aff == NO_AFF || aff == AFF_PM) continue;
		for(j=0;j<_ithis->sd->v2vlist[i].countofvm;j++)
		{
			p_vm1 = _ithis->sd->v2vlist[i].p_vms[j];
			if(p_vm1->at_vmgid == grp_id)
			{
				switch(aff)
				{
				case N_AFF_PM:
					if(j==0) p_vm2 = _ithis->sd->v2vlist[i].p_vms[1];
					else p_vm2 = _ithis->sd->v2vlist[i].p_vms[0];
					pm_id = p_vm2->at_pmid;
					if(pm_id != -1) in_set[pm_id] = FALSE;
					break;
				case AFF_FR:
					for(k=0;k<pm_total;k++) in_set2[k] = FALSE;
					for(k=0;k<_ithis->sd->v2vlist[i].countofvm;k++)
					{
						pm_id = _ithis->sd->v2vlist[i].p_vms[k]->at_pmid;
						if(pm_id != -1)
						{
							p_frame = _ithis->sd->pmlist[pm_id].at_p_fr;
							for(l=0;l<p_frame->countofpm;l++)
							{
								pm_id = p_frame->i_pmids[l];
								in_set2[pm_id] = TRUE;
							}
							break;
						}
					}
					if(k==_ithis->sd->v2vlist[i].countofvm) break;
					for(k=0;k<pm_total;k++) in_set[k] &= in_set2[k];
					break;
				case AFF_GR:
					mark = FALSE;
					for(k=0;k<pm_total;k++) in_set2[k] = FALSE;
					for(k=0;k<_ithis->sd->v2vlist[i].countofvm;k++)
					{
						pm_id = _ithis->sd->v2vlist[i].p_vms[k]->at_pmid;
						if(pm_id != -1)
						{
							for(l=0;l<_ithis->sd->pmlist[pm_id].countofpmgrp;l++)
							{
								p_pmg = _ithis->sd->pmlist[pm_id].at_p_pmgrps[l];
								for(m=0;m<p_pmg->countofpm;m++)
								{
									pm_id = p_pmg->i_pmids[m];
									in_set2[pm_id] = TRUE;
								}
							}
							mark = TRUE;
						}
					}
					if(mark == FALSE) break;
					for(k=0;k<pm_total;k++) in_set[k] &= in_set2[k];
					break;
				case N_AFF_GR:
					if(j==0) p_vm2 = _ithis->sd->v2vlist[i].p_vms[1];
					else p_vm2 = _ithis->sd->v2vlist[i].p_vms[0];
					pm_id = p_vm2->at_pmid;
					if(pm_id != -1)
					{
						for(k=0;k<_ithis->sd->pmlist[pm_id].countofpmgrp;k++)
						{
							p_pmg = _ithis->sd->pmlist[pm_id].at_p_pmgrps[k];
							for(l=0;l<p_pmg->countofpm;l++)
							{
								pm_id = p_pmg->i_pmids[l];
								in_set[pm_id] = FALSE;
							}
						}
					}
					break;
				}
				break;
			}
		}
	}
	for(i=0;i<pm_total;i++)
	{
		if(in_set[i] == TRUE)
		{
			pm_list[count++] = i;
		}
	}
	return count;
}*/

/*
函数名:in_list
参数说明:
host_id:主机（PM或VM）ID
total:host_list元素个数
host_list:主机列表
功能：判断ID为host_id的主机是否在列表中
返回值：pm_list中的元素个数
Create by Liu Yuanqing
*/
int in_list(int host_id,int total,int host_list[])
{
	int i;
	for(i=0;i<total;i++)
		if(host_list[i] == host_id) return i;
	return -1;
}

//void initplace()
//{
	
	//_ithis->part=g_part;

	/*读取XML负载数数据*/
	
	
	/*读取V2V流量关系XML数据及亲和性信息*/
	
	
	/*获取取PM之间时延矩阵*/
	
	
	/*处理VM亲和性与VM分组*/

	
	/*基于分组进行bin_packing，获取放置决策结果*/
	//vm_group_packing();
	
	//return;
//}
/*寻找p_pm所在的超级组，若该超级组已创建并找到，则返回该组的指针，否则（该PM所在超级组未创建）返回NULL*/
hgPtr find_hg(pmPtr p_pm)
{
	BOOL found;
	int i,j;
	BOOL *t;
	for(i=0;i<hg_total;i++)
	{
		if(hglist[i].at_p_fr == p_pm->at_p_fr && hglist[i].countofpmgrp == p_pm->countofpmgrp)//框信息相同且组个数相同
		{
			found = TRUE;//先假定找到
			t = (BOOL *)malloc(sizeof(BOOL)*_ithis->sd->gr_total);//申请一个组总数大小的BOOL数组，用于标记该超级组包含的组
			for(j=0;j<_ithis->sd->gr_total;j++)
				t[j] = FALSE;//初始化标记
			for(j=0;j<hglist[i].countofpmgrp;j++)
			{
				t[hglist[i].at_p_pmgrps[j]->id] = TRUE;//将该超级组包含的组加上标记
			}
			for(j=0;j<p_pm->countofpmgrp;j++)
			{
			if(t[p_pm->at_p_pmgrps[j]->id] == TRUE) continue;//pm所在组与超级组所在组匹配，则继续查找
				else//pm所在的某个组与超级组不匹配
				{
					found = FALSE;
					break;
				}
			}
			free(t);
			if(found == TRUE)//找到匹配的超级组则返回超级组地址
			{
				return &hglist[i];
			}
		}
	}
	return NULL;//未找到返回NULL
}

/*将ID为pm_id的PM加入超级组*/
void add_pm2hg(int pm_id)
{
	hgPtr p_hg;
	pmPtr p_pm;
	p_pm = &_ithis->sd->pmlist[pm_id];
	p_hg = find_hg(p_pm);//查找p_pm指向的PM所在的超级组的指针
	if(p_hg == NULL)//如果该PM所在的超级组未创建，新建一个超级组并将当前PM加入该超级组
	{
		hglist[hg_total].at_p_fr = p_pm->at_p_fr;
		hglist[hg_total].countofpmgrp = p_pm->countofpmgrp;
		memcpy(hglist[hg_total].at_p_pmgrps,p_pm->at_p_pmgrps,sizeof(pmgPtr)*p_pm->countofpmgrp);
		hglist[hg_total].countofpm = 1;
		hglist[hg_total].i_pmids[0] = pm_id;
		hglist[hg_total].id = hg_total;
		p_pm->at_p_hg = &hglist[hg_total];
		hg_total++;
	}
	else//若该PM所在超级组已经创建，则该超级组增加当前PM
	{
		p_hg->i_pmids[p_hg->countofpm++] = pm_id;
		p_pm->at_p_hg = p_hg;
	}
}

void create_hg()
{
	int i;
	hg_total = 0;//超级组总数初始化
	for(i=0;i<_ithis->sd->pm_total;i++)
		add_pm2hg(i);//遍历所有PM，从PM所在组框信息生成或将PM加入超级组
}
/*根据组框亲和性计算p_vmg指向的vmg可以放置的超级组，
将可放置的超级组列表放入p_avail_hg_list指向的数组，
返回可放置超级组个数*/
int get_avail_hg(vmgPtr p_vmg,hgPtr **p_avail_hg_list)
{
	int i,j,k;
	int count = 0;
	BOOL *t;
	int vm_total = p_vmg->vm_total;
	int vm_id,pm_id;
	int ptrnow;
	int *vm_id_list = p_vmg->vm_id_list;
	BOOL *inset;
	vmPtr p_vm1,p_vm2;
	pmPtr p_pm;
	framePtr p_frame;
	pmgPtr p_pmg;
	vm2vmPtr afflist = ini_afflist;
	inset = (BOOL *)malloc(sizeof(BOOL) * hg_total);//申请一个BOOL数组作为超级组可放置的标志
	for(i=0;i<hg_total;i++) inset[i] = TRUE;//初步假定所有超级组都可放置
	if(vm_total == 1)
	{
		p_vm1 = &_ithis->sd->vmlist[vm_id_list[0]];
		if(p_vm1->at_frame_id != -1)//如果有指定框要求
		{
			p_frame = &_ithis->sd->framelist[p_vm1->at_frame_id];
			for(i=0;i<hg_total;i++)
			{
				if(hglist[i].at_p_fr != p_frame) inset[i] = FALSE;//将框信息不匹配的超级组标记FALSE
			}
		}
		if(p_vm1->at_pmg_id != -1)//如果有指定组要求
		{
			p_pmg = &_ithis->sd->pmglist[p_vm1->at_pmg_id];
			for(i=0;i<hg_total;i++)
			{
				for(j=0;j<hglist[i].countofpmgrp;j++)
				{
					if(hglist[i].at_p_pmgrps[j] == p_pmg) break;
				}
				if(j == hglist[i].countofpmgrp) inset[i] = FALSE;//将组信息不匹配的超级组标记为FALSE
			}
		}
	}
	for(i=0;i<vm_total;i++)
	{
		vm_id = vm_id_list[i];
		p_vm1 = &_ithis->sd->vmlist[vm_id];
		/*同框处理*/
		for(ptrnow = p_vm1->aff_fr_head;ptrnow != -1;ptrnow = afflist[ptrnow].nextPtr)
		{
			p_vm2 = afflist[ptrnow].p_vms[1];
			pm_id = p_vm2->at_pmid;
			if(pm_id != -1)
			{
				p_frame = _ithis->sd->pmlist[pm_id].at_p_fr;
				for(j=0;j<hg_total;j++) if(hglist[j].at_p_fr != p_frame) inset[j] = FALSE;//将框信息不匹配的超级组标记FALSE
			}
		}
		/*不同框处理*/
		for(ptrnow = p_vm1->n_aff_fr_head;ptrnow != -1;ptrnow = afflist[ptrnow].nextPtr)
		{
			p_vm2 = afflist[ptrnow].p_vms[1];
			pm_id = p_vm2->at_pmid;
			if(pm_id != -1)
			{
				p_frame = _ithis->sd->pmlist[pm_id].at_p_fr;
				for(j=0;j<hg_total;j++) if(hglist[j].at_p_fr == p_frame) inset[j] = FALSE;//将框信息不匹配的超级组标记FALSE
			}
		}
		/*同组处理*/
		for(ptrnow = p_vm1->aff_gr_head;ptrnow != -1;ptrnow = afflist[ptrnow].nextPtr)
		{
			p_vm2 = afflist[ptrnow].p_vms[1];
			pm_id = p_vm2->at_pmid;
			if(pm_id != -1)
			{
				p_pm = &_ithis->sd->pmlist[pm_id];
				t = (BOOL *)malloc(sizeof(BOOL) * _ithis->sd->gr_total);//申请一个BOOL数组记录当前PM所在的组
				for(j=0;j<_ithis->sd->gr_total;j++) t[j] = FALSE;
				for(j=0;j<p_pm->countofpmgrp;j++)
				{
					t[p_pm->at_p_pmgrps[j]->id] = TRUE;
				}
				for(j=0;j<hg_total;j++)
				{
					if(inset[j] == FALSE) continue;//当前扫描到的超级组已经被标记为不可放置，则跳过
					//找到某个标记为TRUE的超级组
					for(k=0;k<hglist[j].countofpmgrp;k++)
						if(t[hglist[j].at_p_pmgrps[k]->id] == TRUE) break;
					if(k==hglist[j].countofpmgrp) inset[j] = FALSE;//如果当前扫描到的超级组所在的组中没有该PM所在组，则把该超级组标记为FALSE
				}
				free(t);
			}
		}
		/*不同组处理*/
		for(ptrnow = p_vm1->n_aff_gr_head;ptrnow != -1;ptrnow = afflist[ptrnow].nextPtr)
		{
			p_vm2 = afflist[ptrnow].p_vms[1];
			pm_id = p_vm2->at_pmid;
			if(pm_id != -1)
			{
				p_pm = &_ithis->sd->pmlist[pm_id];
				t = (BOOL *)malloc(sizeof(BOOL) * _ithis->sd->gr_total);//申请一个BOOL数组记录当前PM所在的组
				for(j=0;j<_ithis->sd->gr_total;j++) t[j] = FALSE;
				for(j=0;j<p_pm->countofpmgrp;j++)
				{
					t[p_pm->at_p_pmgrps[j]->id] = TRUE;
				}
				for(j=0;j<hg_total;j++)
				{
					if(inset[j] == FALSE) continue;//当前扫描到的超级组已经被标记为不可放置，则跳过
					//找到某个标记为TRUE的超级组
					for(k=0;k<hglist[j].countofpmgrp;k++)
						if(t[hglist[j].at_p_pmgrps[k]->id] == TRUE)//如果当前扫描到的超级组所在的组中有该PM所在组，则把该超级组标记为FALSE
						{
							inset[j] = FALSE;
							break;
						}
				}
				free(t);
			}
		}
	}
	for(i=0;i<hg_total;i++)
	{
		if(inset[i] == TRUE) count++;//记录可放置超级组总数
	}
	if(count != 0)//超级组总数不为0
	{
		*p_avail_hg_list = (hgPtr *)malloc(sizeof(hgPtr)*count);//申请空间存放超级组
		j=0;
		for(i=0;i<hg_total;i++)
		{
			if(inset[i] == TRUE)
			{
				(*p_avail_hg_list)[j] = &hglist[i];
				j++;
			}
		}
	}
	else *p_avail_hg_list = NULL;
	free(inset);
	return count;
}

/*根据不同板亲和性计算p_vmg指向的vmg不可以放置的PM，
将不可放置的PM列表放入p_unavail_pmlist指向的数组，
返回不可放置的PM个数*/
int get_unavail_pm_by_NO_AFF_PM(vmgPtr p_vmg,pmPtr **p_unavail_pmlist)
{
	vm2vmPtr afflist = ini_afflist;
	int ptrnow,i,pm_id;
	pmPtr *unavail_list;
	int count = 0;
	vmPtr p_vm1,p_vm2;
	unavail_list = (pmPtr *)malloc(sizeof(pmPtr) * _ithis->sd->pm_total);
	
	for(i=0;i<p_vmg->vm_total;i++)
	{
		p_vm1 = &_ithis->sd->vmlist[p_vmg->vm_id_list[i]];
		for(ptrnow = p_vm1->n_aff_pm_head;ptrnow != -1;ptrnow = afflist[ptrnow].nextPtr)
		{
			p_vm2 = afflist[ptrnow].p_vms[1];
			pm_id = p_vm2->at_pmid;
			if(pm_id != -1)
			{
				unavail_list[count++] = &_ithis->sd->pmlist[pm_id]; 
			}
		}
	}
	if(count != 0)
	{
		*p_unavail_pmlist = (pmPtr *)malloc(sizeof(pmPtr) * count);
		memcpy(*p_unavail_pmlist,unavail_list,sizeof(pmPtr) * count);
	}
	else *p_unavail_pmlist = NULL;
	free(unavail_list);
	return count;
}

//判断ID为pm_id的PM是否在不可放置列表pmlist中
BOOL isavail(int pm_id,pmPtr pmlist[],int pm_total)
{
	int i;
	for(i=0;i<pm_total;i++)
	{
		if(pmlist[i]->host.id == pm_id) return FALSE;
	}
	return TRUE;
}

/*递归放置超级组*/
/*
参数说明：
vmg_now:当前要放置的vmg在数组p_vmg中的下标，同时也是已经放置的vmg个数
p_vmg:待放置的VMG的指针数组
vmg_total:VMG总数
返回值：TRUE表示放置成功，FALSE表示亲和性不满足造成放置失败，OVERLOAD表示剩余资源不足导致放置失败
*/
BOOL placeVmg(int vmg_now,vmgPtr p_vmg[],int vmg_total)
{
	BOOL place_ok;
	int avail_hg_total;
	int unavail_pm_total;
	int i,j,k;
	int pm_id;
	pmPtr p_pm;
	hgPtr *avail_hglist;
	pmPtr *unavail_pmlist;
	vmgPtr ppart = _ithis->sd->vmglist;
	if(vmg_now == vmg_total) return TRUE;//递归出口，当成功放置的VMG个数与总数相等，返回true
	if(p_vmg[vmg_now]->aff_pm != FALSE)//该组有指定板的要求
	{
		if(can_place(p_vmg[vmg_now]->groupid,p_vmg[vmg_now]->at_pmid,_ithis->sd->hot_limit,ppart))//指定的板可以放置，则先放置在此
		{
			pm_id = p_vmg[vmg_now]->at_pmid;
			for(j=0;j<8;j++){
				_ithis->sd->pmlist[pm_id].host.load[j].used 
					+= p_vmg[vmg_now]->load[j].total;//更新负载
			}
			set_at_pmid(p_vmg[vmg_now]->groupid,ppart);
		}
		else return OVERLOAD;//指定的板剩余资源不足，返回超载
		place_ok = placeVmg(vmg_now + 1,p_vmg,vmg_total);//尝试放置下一组
		if(place_ok != TRUE)//如果放置失败
		{
			for(j=0;j<8;j++){
				_ithis->sd->pmlist[pm_id].host.load[j].used 
					-= p_vmg[vmg_now]->load[j].total;//重置负载
			}
			return place_ok;//返回放置失败的信息
		}
		else return TRUE;//放置成功，返回TRUE
	}
	//若待放置的vmg没有指定板要求
	avail_hg_total = get_avail_hg(p_vmg[vmg_now],&avail_hglist);//计算当前vmg可以放置的超级组，放到avail_hglist中
	if(avail_hg_total == 0)//没有可用的超级组，即亲和性约束造成无可放置的超级组，返回FALSE
	{
		return FALSE;
	}
	unavail_pm_total = get_unavail_pm_by_NO_AFF_PM(p_vmg[vmg_now],&unavail_pmlist);//计算当前vmg不可放置的PM，放到unavail_pmlist中
	for(i=0;i<avail_hg_total;i++)//遍历所有可放置的超级组
	{
		for(j=0;j<avail_hglist[i]->countofpm;j++)//遍历超级组中的所有PM
		{
			pm_id = avail_hglist[i]->i_pmids[j];
			if(isavail(pm_id,unavail_pmlist,unavail_pm_total) && can_place(p_vmg[vmg_now]->groupid,pm_id,_ithis->sd->hot_limit,ppart))//如果当前PM余量足够并且不在不可放置列表中
			{
				//采用FIRST FIT策略选择物理机
				p_vmg[vmg_now]->at_pmid = pm_id;/*设置当前组对应的物理机*/

				for(k=0;k<8;k++){
					_ithis->sd->pmlist[pm_id].host.load[k].used 
						+= p_vmg[vmg_now]->load[k].total;//更新负载
				}
				set_at_pmid(p_vmg[vmg_now]->groupid,ppart);
			}
			else continue;
			place_ok = placeVmg(vmg_now + 1,p_vmg,vmg_total);//尝试放置下一组
			if(place_ok == FALSE)//由于亲和性造成不可放置，则跳出遍历PM的循环，进入下一个超级组
			{
				p_vmg[vmg_now]->at_pmid = -1;/*还原当前组对应的物理机*/

				for(k=0;k<8;k++){
					_ithis->sd->pmlist[pm_id].host.load[k].used 
						-= p_vmg[vmg_now]->load[k].total;//重置负载
				}
				set_at_pmid(p_vmg[vmg_now]->groupid,ppart);
				break;
			}
			else if(place_ok == OVERLOAD)//由于超载造成不可放置，则选择该超级组中下一个PM
			{
				p_vmg[vmg_now]->at_pmid = -1;/*设置当前组对应的物理机*/

				for(k=0;k<8;k++){
					_ithis->sd->pmlist[pm_id].host.load[k].used 
						-= p_vmg[vmg_now]->load[k].total;//更新负载
				}
				set_at_pmid(p_vmg[vmg_now]->groupid,ppart);
			}
			else//放置成功
			{
				if(avail_hglist != NULL) free(avail_hglist);
				if(unavail_pmlist != NULL) free(unavail_pmlist);
				return TRUE;
			}
		}
	}
	if(avail_hglist != NULL) free(avail_hglist);
	if(unavail_pmlist != NULL) free(unavail_pmlist);
	return OVERLOAD;//如果所有PM均尝试过，则为超载
}

/*递归放置算法*/
/*p_vmg:待放置的vmg列表，vmg_total:vmg总数*/
void NP_place(vmgPtr *p_vmg,int vmg_total)
{
	BOOL place_ok;
	int i,j;
	int pm_id;
	create_hg();//创建超级组
	for(i=0;i<vmg_total;i++)//将at_pmid还原为原状态
	{
		pm_id = p_vmg[i]->at_pmid;
		if(p_vmg[i]->aff_pm == FALSE)//若该VMG没有指定板，则其at_pmid还原为-1
		{
			p_vmg[i]->at_pmid = -1;
			set_at_pmid(p_vmg[i]->groupid,_ithis->sd->vmglist);
		}
		for(j=0;j<8;j++)//还原负载
		{
			_ithis->sd->pmlist[pm_id].host.load[j].used 
			-= p_vmg[i]->load[j].total;//重置负载
		}
	}
	placeVmg(0,p_vmg,vmg_total);//从第0个开始放置
}