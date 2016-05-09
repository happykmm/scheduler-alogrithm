/*
  Module: Initplace module
  Ҫ�������ȡ�������ݣ���ȡ�������ݣ���ȡPMʱ�Ӿ���
  ��������ý��
  Created by chenjianhai.
  Created date: 2013/04/07
*/

#include "alg_initplace.h"
#include <string.h>
#include <time.h>
/*
@�������ܣ����ȷ����㷨ʵ�ֳ��Է��þ���
����˵����THIS ָ���Լ���ָ��
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


/*��ʼ���㷨���ò�������XML11,CJH ADD*/
void setRequest_initplace(cls_initplacePtr THIS)
{
	/*��ȡ���ò���*/
	/*THIS->hot=THIS->sd->req_cs->alg.para[0].para_value;
	THIS->cold=THIS->sd->req_cs->alg.para[1].para_value;
	THIS->warm=THIS->sd->req_cs->alg.para[2].para_value;*/
	//int i;
	//for(i=0;i<DIM;i++)
		//THIS->hot[i] = THIS->sd->req_cs->alg.para[0].para_double_arr[i];

	/*�ڸú������Ѿ���hot_limit, cold_limit��������sharedData�У�
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

/*��ȡdelay����*/
/*
������:get_delay
����˵��:
pm_total:pm_list��Ԫ�ظ���
pm_list[]��������б�
���ܣ�����pm_list��Ӧ��ʱ�Ӿ���pmdelay
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
������:init_at_pmid
���ܣ���ʼ������VM���ڵ�PMΪ-1����ʾ����ʼ���ã�
Create by Liu Yuanqing
*/
void init_at_pmid()
{
	int i;
	for(i=0;i<_ithis->sd->vm_total;i++) _ithis->sd->vmlist[i].at_pmid = -1;
}


/*
������:set_at_pmid
����˵��:
part_no:�����������õ�PM���������ı��
ppart:��������б�
���ܣ�Ϊ���Ϊpart_no����������е�����������������ڵ�PM����PMΪ��������PM
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
/*���ܣ���������г��Է��ã���������ʽ��װ�䷽��
  ppart: ָ������������ݽṹ��ָ�룺g_part
  ������2013-05-15 by JianhaiChen
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

	//�����ڲ�����
	//compute_itraffic();

	//�����Ƿ��в�ͬ���׺�������
	//sort_by_traffic();

	//css.get_max_vec(dimMax);

	//�ж��ڲ�n_aff��ͻ
	for (i=0;i < _ithis->sd->vmg_total;i++){
		do{
			pre_vmg_total = vmg_total;
			vmg_total = check_naff(i,_ithis->sd->vmglist,vmg_total,vmg_list);
		}while(vmg_total!=pre_vmg_total);

		//�жϸ������Ƿ��ܷŵ������������
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
		//ָ������ᱻrandom_shuffle Ӧ�����±���
		
		for (i = 0;i <_ithis->sd->vmg_total;i++)
			p_vmg[i] = &_ithis->sd->vmglist[i];
		css.placeVMGtoPM(p_vmg,_ithis->sd->vmg_total,
			p_pms, _ithis->sd->pm_total,
			_ithis->sd->v2plist, &_ithis->sd->v2p_total);
		if (_ithis->sd->v2p_total==-1){
			//����ʧ��
			order = (int *)malloc(sizeof(int)*_ithis->sd->vmg_total);

			//order�����ʾ������PM����С���������VMG���
			css.getMinAvailCountOfVmg(queue_vmg, _ithis->sd->vmg_total, order);

			singleVMG = 0;
			//�����и��������vmg_queue
			for (i= 0 ;i < css.maxSearchDepth+1; i++){
				if (_ithis->sd->vmglist[queue_vmg[order[i]]->groupid].vm_total>1){
					cutRes = group_cut(queue_vmg[order[i]]->groupid,_ithis->sd->vmglist);
					if (cutRes!=INF){
						break;
					}
				}else singleVMG ++;
			}
			//�������и�и������������
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
			//���vmgroupȫ�������и��� ���н���
			
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
			printf("�������%d�޷�װ�䣬��Ҫ�¿������\n",i);
			ppart[i].at_pmid = -1;
			continue;
		}*/
		//find_min_star(i,pm_total,pm_list); //������Сʱ���ҳ�һ��VM��

		place_ok &= pack_group(i,_ithis->sd->hot_limit,ppart);
	}
	if(place_ok == FALSE)/*�ǵݹ��޷����ã����õݹ�ķ���*/
	{
		p_pms = (pmPtr *)malloc(sizeof(pmPtr) * _ithis->sd->pm_total);
		for(i=0;i<_ithis->sd->pm_total;i++)
		{
			p_pms[i] = &_ithis->sd->pmlist[i];
		}
		//placeVMGtoPM(p_vmg,_ithis->sd->vmg_total,p_pms,_ithis->sd->pm_total,v2plist,&v2p_total);
		NP_place(p_vmg,_ithis->sd->vmg_total);//�ݹ����VMG�������
		free(p_pms);
	}
}

/********************************************
@ ���ܣ��жϸ����ĸ��������Ƿ��ܷŵ���ppm
@ ������ppm������PM��load: ��������ָ�룬��Ϊvm,���ж�VM�Ƿ��ܷţ�
��Ϊvmg�����ж�vmg�Ƿ��ܷ�, dim-ά�ȣ�0��dim-1
@ �����ߣ�by Jianhai Chen
/********************************************/
int canpack(pmPtr ppm, st_metric *load,int dim)
{
	double remained;
	int j;
	int packed=1;/*��ʼ�ٶ��ǿ��Էŵ�1*/
	
	for(j=0;j<dim;j++) 
	{
		/*��jά�ȣ���������ppm��ʣ����Դ*/
		remained =ppm->host.load[j].total- ppm->host.load[j].reserved - ppm->host.load[j].used; 
		
		/*�Ƚϣ�����Ų��£����ʾ�Ų���ȥ*/
		if(remained <= load[j].used) packed=0;		
	}
	
	return packed;
}


/*��ȡ�������ʣ����Դ*/
/*
������:get_remained
����˵��:
ppm:�����ָ��
dim��ά��
���ܣ�����ppmָ����������dimά�ȵ�ʣ����Դ
����ֵ��ppmָ����������dimά�ȵ�ʣ����Դ
Create by Liu Yuanqing
*/
double get_remained(pmPtr ppm,int dim)
{	double ret;
	ret=ppm->host.load[dim].total - ppm->host.load[dim].reserved - ppm->host.load[dim].used;
	return ret;
}

/*pm\vm\vmg������Դά�����
type=1 -VM,vmg, type=2, pm*/
//����������Ըɵ���~
double get_sum_net(st_metric *pload, int type)
{
	double ret=0;
	int j;
	
	/*ά��4-8��������Դ*/
	for(j=4;j<8;j++){
		if(type == T_VM || type == T_VMG) //vm,vmg
			ret += pload[j].used;
		else  //pm
			ret += pload[j].total - pload[j].reserved - pload[j].used;
	}

	return ret;
}

/*�����ڲ�����*/
/*
������:compute_itraffic
���ܣ������������������ڲ�����������¼������group_traffic��
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


/*���ڲ���������*/
/*
������:sort_by_traffic
���ܣ��������������ڲ��������������������������
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

/*��ʱ������*/
/*
������:sort_by_delay
����˵��:
src_id:��ΪԴ�������ID
pm_total:������б�pm_list��Ԫ�ظ���
pm_list:������б�
���ܣ�����������б��е��������Դ�������ʱ�Ӱ���������������
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

/*��ȡ�������ά��Դ*/
/*
������:get_pm_source
����˵��:
pm_total:������б�pm_list��Ԫ�ظ���
pm_list:������б�
���ܣ��������������ÿһά�ȵ�ʣ������ƽ��ֵΪ��׼��
����������н�ά����8ά��Ϊ1ά�����Ա���ȫ�����һ��
�����ʣ����Դ�Ķ��١���ά�����Դ��С������pm_source
�����С�
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




/*������vmg���������жϸ�����һ��PM��Դ�Ƿ��ܹ��ŵ��£���PM��Դ��1.5��������*/
/*
������:pm_grouped
����˵��:
vmg_load:���������ܸ���
pm_load:����������ʣ����Դ
���ܣ����������ÿһά�ȵ���ʣ���������������
ÿһά���ܸ��ص�1.5��ʱ�������жϸ���������ܷ�
��
����ֵ���������ж��ܷ��·���TRUE�����򷵻�FALSE
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


/*FindMinStar�㷨Ѱ���ʺϵ��������*/
/*
������:find_min_star
����˵��:
grp_id:�����õ���������ID
pm_total:������б�pm_list��Ԫ�ظ���
pm_list:������б�
���ܣ���pm_list����ѡһϵ���������Ϊ���÷�Χ
��ѡԭ������������ΪԴ��������ѡʱ��С
����������������ж���һϵ��������ɷ��¸�
�������ʱ��ֹͣ�㷨������ѡ�����������С
�������ID��������pm_group;
����ֵ���������ж��ܷ��·���TRUE�����򷵻�FALSE
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
  for(j=1;j<pm_total;j++)//Ѱ����������
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
  total_pm_source.eth3 += get_remained(p_pm,ETH3);//������������ΪԴ

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
	  && pm_num < pm_total){//��1.5��������鸺��Ϊ��׼�����������
  
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
	  
  //ѡ������
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
  } //��������鰴��С����*/
}

/*
������:check_naff
����˵����
ppart:��������б�
part_no:����鷴�׺��Ե��������ID
���ܣ��������������Ƿ���Υ�����׺���ԭ���VM�ԣ�
��������п����׺�VM���γ�����
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
	for(i=1;i<pm_total;i++)//Ѱ����������
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

	/*�жϻ���ά���Ƿ��װ*/
	for(j=0;j<8;j++){
		avail = _ithis->sd->pmlist[pm_no].host.load[j].total - _ithis->sd->pmlist[pm_no].host.load[j].reserved;
		if(avail>_ithis->sd->pmlist[pm_no].host.load[j].total * hot[j]) 
			avail = _ithis->sd->pmlist[pm_no].host.load[j].total * hot[j];
		//remained = sh.getTotal(j) * HOT - sh.getUsed(j);/*��ȡ�������ȵ��ʣ����Դ��С���޸���8-13��by sun*/
		remained =avail - _ithis->sd->pmlist[pm_no].host.load[j].used;
		if(ppart[part_no].load[j].total > remained) return FALSE;/*�ж��Ƿ��װ*/
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
 * �Ա��Ϊpart_no�������װ����ߣ�����bestfit
 * @param part_no: ����� 
 * @param ppart��ָ�룬ָ�������������ϵ���� ���ɵ�����
 * @ �޷���
 * @created by jianhai at 2013.5.12
 * @modified by jianhai at 2013.5.21
 */
/*�Ե�ǰ�����װ��*/
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
	//double remained;//ʣ����
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
			for(j=0;j<pm_total;j++)//�������������
			{
				packed = can_place(vmg_list[i],pm_list[j],hot,ppart);
				if(packed)//�����װ������FIRST FIT
				{
					pm_no = pm_list[j];
					break;
				}
			}
			if(pm_no != -1){ //�ҵ�ĳһ̨�������װ

				ppart[vmg_list[i]].at_pmid = pm_no;/*���õ�ǰ���Ӧ�������*/

				for(j=0;j<8;j++){
					_ithis->sd->pmlist[pm_no].host.load[j].used 
						+= ppart[vmg_list[i]].load[j].total;//���¸���
				}
				set_at_pmid(vmg_list[i],ppart);
			}
			else
			{
				if(ppart[vmg_list[i]].vm_total > 1)//�������������1̨�ſ��и�
				{
					min_cut = group_cut(vmg_list[i],ppart);//�޿�װ��������������и�
					if(min_cut < INF)//����δ��ͬ���׺������ƣ����и�
					{
						//pack_group(_ithis->sd->vmg_total-1,ppart);//���и�����������װ��
						vmg_list[vmg_total++] = _ithis->sd->vmg_total-1;
					}
					else
					{
						printf("�������%d�޷����ã���Ҫ�¿������\n",vmg_list[i]);
						ppart[vmg_list[i]].at_pmid = -1;
						return FALSE;
					}
				}
				else
				{
					printf("�������%d�޷����ã���Ҫ�¿������\n",vmg_list[i]);
					ppart[vmg_list[i]].at_pmid = -1;
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

/*
������:get_available_pm
����˵��:
grp_id:�����õ��������ID
pm_list:�ɷ��õ�PM�б�
���ܣ������׺����뷴�׺��Լ���IDΪgrp_id�����������Է��õ��������Χ
����ֵ��pm_list�е�Ԫ�ظ���
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
		/*��ͬ�崦��*/
		for(ptrnow = p_vm1->n_aff_pm_head;ptrnow != -1;ptrnow = afflist[ptrnow].nextPtr)
		{
			p_vm2 = afflist[ptrnow].p_vms[1];
			pm_id = p_vm2->at_pmid;
			if(pm_id != -1) in_set[pm_id] = FALSE;
		}
		/*ͬ����*/
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
		/*��ͬ����*/
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
		/*ͬ�鴦��*/
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
		/*��ͬ�鴦��*/
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
������:in_list
����˵��:
host_id:������PM��VM��ID
total:host_listԪ�ظ���
host_list:�����б�
���ܣ��ж�IDΪhost_id�������Ƿ����б���
����ֵ��pm_list�е�Ԫ�ظ���
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

	/*��ȡXML����������*/
	
	
	/*��ȡV2V������ϵXML���ݼ��׺�����Ϣ*/
	
	
	/*��ȡȡPM֮��ʱ�Ӿ���*/
	
	
	/*����VM�׺�����VM����*/

	
	/*���ڷ������bin_packing����ȡ���þ��߽��*/
	//vm_group_packing();
	
	//return;
//}
/*Ѱ��p_pm���ڵĳ����飬���ó������Ѵ������ҵ����򷵻ظ����ָ�룬���򣨸�PM���ڳ�����δ����������NULL*/
hgPtr find_hg(pmPtr p_pm)
{
	BOOL found;
	int i,j;
	BOOL *t;
	for(i=0;i<hg_total;i++)
	{
		if(hglist[i].at_p_fr == p_pm->at_p_fr && hglist[i].countofpmgrp == p_pm->countofpmgrp)//����Ϣ��ͬ���������ͬ
		{
			found = TRUE;//�ȼٶ��ҵ�
			t = (BOOL *)malloc(sizeof(BOOL)*_ithis->sd->gr_total);//����һ����������С��BOOL���飬���ڱ�Ǹó������������
			for(j=0;j<_ithis->sd->gr_total;j++)
				t[j] = FALSE;//��ʼ�����
			for(j=0;j<hglist[i].countofpmgrp;j++)
			{
				t[hglist[i].at_p_pmgrps[j]->id] = TRUE;//���ó��������������ϱ��
			}
			for(j=0;j<p_pm->countofpmgrp;j++)
			{
			if(t[p_pm->at_p_pmgrps[j]->id] == TRUE) continue;//pm�������볬����������ƥ�䣬���������
				else//pm���ڵ�ĳ�����볬���鲻ƥ��
				{
					found = FALSE;
					break;
				}
			}
			free(t);
			if(found == TRUE)//�ҵ�ƥ��ĳ������򷵻س������ַ
			{
				return &hglist[i];
			}
		}
	}
	return NULL;//δ�ҵ�����NULL
}

/*��IDΪpm_id��PM���볬����*/
void add_pm2hg(int pm_id)
{
	hgPtr p_hg;
	pmPtr p_pm;
	p_pm = &_ithis->sd->pmlist[pm_id];
	p_hg = find_hg(p_pm);//����p_pmָ���PM���ڵĳ������ָ��
	if(p_hg == NULL)//�����PM���ڵĳ�����δ�������½�һ�������鲢����ǰPM����ó�����
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
	else//����PM���ڳ������Ѿ���������ó��������ӵ�ǰPM
	{
		p_hg->i_pmids[p_hg->countofpm++] = pm_id;
		p_pm->at_p_hg = p_hg;
	}
}

void create_hg()
{
	int i;
	hg_total = 0;//������������ʼ��
	for(i=0;i<_ithis->sd->pm_total;i++)
		add_pm2hg(i);//��������PM����PM���������Ϣ���ɻ�PM���볬����
}
/*��������׺��Լ���p_vmgָ���vmg���Է��õĳ����飬
���ɷ��õĳ������б����p_avail_hg_listָ������飬
���ؿɷ��ó��������*/
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
	inset = (BOOL *)malloc(sizeof(BOOL) * hg_total);//����һ��BOOL������Ϊ������ɷ��õı�־
	for(i=0;i<hg_total;i++) inset[i] = TRUE;//�����ٶ����г����鶼�ɷ���
	if(vm_total == 1)
	{
		p_vm1 = &_ithis->sd->vmlist[vm_id_list[0]];
		if(p_vm1->at_frame_id != -1)//�����ָ����Ҫ��
		{
			p_frame = &_ithis->sd->framelist[p_vm1->at_frame_id];
			for(i=0;i<hg_total;i++)
			{
				if(hglist[i].at_p_fr != p_frame) inset[i] = FALSE;//������Ϣ��ƥ��ĳ�������FALSE
			}
		}
		if(p_vm1->at_pmg_id != -1)//�����ָ����Ҫ��
		{
			p_pmg = &_ithis->sd->pmglist[p_vm1->at_pmg_id];
			for(i=0;i<hg_total;i++)
			{
				for(j=0;j<hglist[i].countofpmgrp;j++)
				{
					if(hglist[i].at_p_pmgrps[j] == p_pmg) break;
				}
				if(j == hglist[i].countofpmgrp) inset[i] = FALSE;//������Ϣ��ƥ��ĳ�������ΪFALSE
			}
		}
	}
	for(i=0;i<vm_total;i++)
	{
		vm_id = vm_id_list[i];
		p_vm1 = &_ithis->sd->vmlist[vm_id];
		/*ͬ����*/
		for(ptrnow = p_vm1->aff_fr_head;ptrnow != -1;ptrnow = afflist[ptrnow].nextPtr)
		{
			p_vm2 = afflist[ptrnow].p_vms[1];
			pm_id = p_vm2->at_pmid;
			if(pm_id != -1)
			{
				p_frame = _ithis->sd->pmlist[pm_id].at_p_fr;
				for(j=0;j<hg_total;j++) if(hglist[j].at_p_fr != p_frame) inset[j] = FALSE;//������Ϣ��ƥ��ĳ�������FALSE
			}
		}
		/*��ͬ����*/
		for(ptrnow = p_vm1->n_aff_fr_head;ptrnow != -1;ptrnow = afflist[ptrnow].nextPtr)
		{
			p_vm2 = afflist[ptrnow].p_vms[1];
			pm_id = p_vm2->at_pmid;
			if(pm_id != -1)
			{
				p_frame = _ithis->sd->pmlist[pm_id].at_p_fr;
				for(j=0;j<hg_total;j++) if(hglist[j].at_p_fr == p_frame) inset[j] = FALSE;//������Ϣ��ƥ��ĳ�������FALSE
			}
		}
		/*ͬ�鴦��*/
		for(ptrnow = p_vm1->aff_gr_head;ptrnow != -1;ptrnow = afflist[ptrnow].nextPtr)
		{
			p_vm2 = afflist[ptrnow].p_vms[1];
			pm_id = p_vm2->at_pmid;
			if(pm_id != -1)
			{
				p_pm = &_ithis->sd->pmlist[pm_id];
				t = (BOOL *)malloc(sizeof(BOOL) * _ithis->sd->gr_total);//����һ��BOOL�����¼��ǰPM���ڵ���
				for(j=0;j<_ithis->sd->gr_total;j++) t[j] = FALSE;
				for(j=0;j<p_pm->countofpmgrp;j++)
				{
					t[p_pm->at_p_pmgrps[j]->id] = TRUE;
				}
				for(j=0;j<hg_total;j++)
				{
					if(inset[j] == FALSE) continue;//��ǰɨ�赽�ĳ������Ѿ������Ϊ���ɷ��ã�������
					//�ҵ�ĳ�����ΪTRUE�ĳ�����
					for(k=0;k<hglist[j].countofpmgrp;k++)
						if(t[hglist[j].at_p_pmgrps[k]->id] == TRUE) break;
					if(k==hglist[j].countofpmgrp) inset[j] = FALSE;//�����ǰɨ�赽�ĳ��������ڵ�����û�и�PM�����飬��Ѹó�������ΪFALSE
				}
				free(t);
			}
		}
		/*��ͬ�鴦��*/
		for(ptrnow = p_vm1->n_aff_gr_head;ptrnow != -1;ptrnow = afflist[ptrnow].nextPtr)
		{
			p_vm2 = afflist[ptrnow].p_vms[1];
			pm_id = p_vm2->at_pmid;
			if(pm_id != -1)
			{
				p_pm = &_ithis->sd->pmlist[pm_id];
				t = (BOOL *)malloc(sizeof(BOOL) * _ithis->sd->gr_total);//����һ��BOOL�����¼��ǰPM���ڵ���
				for(j=0;j<_ithis->sd->gr_total;j++) t[j] = FALSE;
				for(j=0;j<p_pm->countofpmgrp;j++)
				{
					t[p_pm->at_p_pmgrps[j]->id] = TRUE;
				}
				for(j=0;j<hg_total;j++)
				{
					if(inset[j] == FALSE) continue;//��ǰɨ�赽�ĳ������Ѿ������Ϊ���ɷ��ã�������
					//�ҵ�ĳ�����ΪTRUE�ĳ�����
					for(k=0;k<hglist[j].countofpmgrp;k++)
						if(t[hglist[j].at_p_pmgrps[k]->id] == TRUE)//�����ǰɨ�赽�ĳ��������ڵ������и�PM�����飬��Ѹó�������ΪFALSE
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
		if(inset[i] == TRUE) count++;//��¼�ɷ��ó���������
	}
	if(count != 0)//������������Ϊ0
	{
		*p_avail_hg_list = (hgPtr *)malloc(sizeof(hgPtr)*count);//����ռ��ų�����
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

/*���ݲ�ͬ���׺��Լ���p_vmgָ���vmg�����Է��õ�PM��
�����ɷ��õ�PM�б����p_unavail_pmlistָ������飬
���ز��ɷ��õ�PM����*/
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

//�ж�IDΪpm_id��PM�Ƿ��ڲ��ɷ����б�pmlist��
BOOL isavail(int pm_id,pmPtr pmlist[],int pm_total)
{
	int i;
	for(i=0;i<pm_total;i++)
	{
		if(pmlist[i]->host.id == pm_id) return FALSE;
	}
	return TRUE;
}

/*�ݹ���ó�����*/
/*
����˵����
vmg_now:��ǰҪ���õ�vmg������p_vmg�е��±꣬ͬʱҲ���Ѿ����õ�vmg����
p_vmg:�����õ�VMG��ָ������
vmg_total:VMG����
����ֵ��TRUE��ʾ���óɹ���FALSE��ʾ�׺��Բ�������ɷ���ʧ�ܣ�OVERLOAD��ʾʣ����Դ���㵼�·���ʧ��
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
	if(vmg_now == vmg_total) return TRUE;//�ݹ���ڣ����ɹ����õ�VMG������������ȣ�����true
	if(p_vmg[vmg_now]->aff_pm != FALSE)//������ָ�����Ҫ��
	{
		if(can_place(p_vmg[vmg_now]->groupid,p_vmg[vmg_now]->at_pmid,_ithis->sd->hot_limit,ppart))//ָ���İ���Է��ã����ȷ����ڴ�
		{
			pm_id = p_vmg[vmg_now]->at_pmid;
			for(j=0;j<8;j++){
				_ithis->sd->pmlist[pm_id].host.load[j].used 
					+= p_vmg[vmg_now]->load[j].total;//���¸���
			}
			set_at_pmid(p_vmg[vmg_now]->groupid,ppart);
		}
		else return OVERLOAD;//ָ���İ�ʣ����Դ���㣬���س���
		place_ok = placeVmg(vmg_now + 1,p_vmg,vmg_total);//���Է�����һ��
		if(place_ok != TRUE)//�������ʧ��
		{
			for(j=0;j<8;j++){
				_ithis->sd->pmlist[pm_id].host.load[j].used 
					-= p_vmg[vmg_now]->load[j].total;//���ø���
			}
			return place_ok;//���ط���ʧ�ܵ���Ϣ
		}
		else return TRUE;//���óɹ�������TRUE
	}
	//�������õ�vmgû��ָ����Ҫ��
	avail_hg_total = get_avail_hg(p_vmg[vmg_now],&avail_hglist);//���㵱ǰvmg���Է��õĳ����飬�ŵ�avail_hglist��
	if(avail_hg_total == 0)//û�п��õĳ����飬���׺���Լ������޿ɷ��õĳ����飬����FALSE
	{
		return FALSE;
	}
	unavail_pm_total = get_unavail_pm_by_NO_AFF_PM(p_vmg[vmg_now],&unavail_pmlist);//���㵱ǰvmg���ɷ��õ�PM���ŵ�unavail_pmlist��
	for(i=0;i<avail_hg_total;i++)//�������пɷ��õĳ�����
	{
		for(j=0;j<avail_hglist[i]->countofpm;j++)//�����������е�����PM
		{
			pm_id = avail_hglist[i]->i_pmids[j];
			if(isavail(pm_id,unavail_pmlist,unavail_pm_total) && can_place(p_vmg[vmg_now]->groupid,pm_id,_ithis->sd->hot_limit,ppart))//�����ǰPM�����㹻���Ҳ��ڲ��ɷ����б���
			{
				//����FIRST FIT����ѡ�������
				p_vmg[vmg_now]->at_pmid = pm_id;/*���õ�ǰ���Ӧ�������*/

				for(k=0;k<8;k++){
					_ithis->sd->pmlist[pm_id].host.load[k].used 
						+= p_vmg[vmg_now]->load[k].total;//���¸���
				}
				set_at_pmid(p_vmg[vmg_now]->groupid,ppart);
			}
			else continue;
			place_ok = placeVmg(vmg_now + 1,p_vmg,vmg_total);//���Է�����һ��
			if(place_ok == FALSE)//�����׺�����ɲ��ɷ��ã�����������PM��ѭ����������һ��������
			{
				p_vmg[vmg_now]->at_pmid = -1;/*��ԭ��ǰ���Ӧ�������*/

				for(k=0;k<8;k++){
					_ithis->sd->pmlist[pm_id].host.load[k].used 
						-= p_vmg[vmg_now]->load[k].total;//���ø���
				}
				set_at_pmid(p_vmg[vmg_now]->groupid,ppart);
				break;
			}
			else if(place_ok == OVERLOAD)//���ڳ�����ɲ��ɷ��ã���ѡ��ó���������һ��PM
			{
				p_vmg[vmg_now]->at_pmid = -1;/*���õ�ǰ���Ӧ�������*/

				for(k=0;k<8;k++){
					_ithis->sd->pmlist[pm_id].host.load[k].used 
						-= p_vmg[vmg_now]->load[k].total;//���¸���
				}
				set_at_pmid(p_vmg[vmg_now]->groupid,ppart);
			}
			else//���óɹ�
			{
				if(avail_hglist != NULL) free(avail_hglist);
				if(unavail_pmlist != NULL) free(unavail_pmlist);
				return TRUE;
			}
		}
	}
	if(avail_hglist != NULL) free(avail_hglist);
	if(unavail_pmlist != NULL) free(unavail_pmlist);
	return OVERLOAD;//�������PM�����Թ�����Ϊ����
}

/*�ݹ�����㷨*/
/*p_vmg:�����õ�vmg�б�vmg_total:vmg����*/
void NP_place(vmgPtr *p_vmg,int vmg_total)
{
	BOOL place_ok;
	int i,j;
	int pm_id;
	create_hg();//����������
	for(i=0;i<vmg_total;i++)//��at_pmid��ԭΪԭ״̬
	{
		pm_id = p_vmg[i]->at_pmid;
		if(p_vmg[i]->aff_pm == FALSE)//����VMGû��ָ���壬����at_pmid��ԭΪ-1
		{
			p_vmg[i]->at_pmid = -1;
			set_at_pmid(p_vmg[i]->groupid,_ithis->sd->vmglist);
		}
		for(j=0;j<8;j++)//��ԭ����
		{
			_ithis->sd->pmlist[pm_id].host.load[j].used 
			-= p_vmg[i]->load[j].total;//���ø���
		}
	}
	placeVmg(0,p_vmg,vmg_total);//�ӵ�0����ʼ����
}