

/*
@函数功能：节能整合算法实现决策
参数说明：
*/

#include "alg_coldsolve.h"

//初始化
void init_coldsolve(cls_coldsolvePtr THIS){
	_csthis=THIS;
	//THIS->setRequest_coldsolve=setRequest_coldsolve;
	THIS->coldsolve=coldsolve;
}

/*初始化算法配置参数来自XML11,CJH ADD*/
void setRequest_coldsolve(cls_coldsolvePtr THIS)
{
	/*获取配置参数*/
	/*st.hot = THIS->hot=THIS->sd->req_cs->alg.para[0].para_value;
	st.cold = THIS->cold=THIS->sd->req_cs->alg.para[1].para_value;
	THIS->warm=THIS->sd->req_cs->alg.para[2].para_value;*/

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
	
		if (strcmp(p_req->type, "CONSOLIDATE")==0){

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

int if_can_off (int pm_id, st_pm pmlist[ ]){
	int i,flag=1;
	for(i=0;i<pmlist[pm_id].countofvm;i++)
		if(pmlist[pm_id].p_vms[i]->host.stat==2)
		{flag=-1;break;}

	return flag;
}

/*
void replace_vm (int group_id, int pm_id){
	int i;
	for(i=0;i<_csthis->sd->vmg_total)
	_csthis->sd->vmglist [i].groupid=
}
*/

/****************************************************
@函数名称：消除冷点
@参数：
@Created by### at ####-##-##
@modified by #### at ####-##-##
****************************************************/
pmPtr wait_pm[PM_TOTAL]; 
vmgPtr wait_vmg[VM_TOTAL];
void coldsolve ()
{
	// num exm1 [PM_TOTAL];   //按照资源大小顺序排列的物理机列表       包含一维资源大小，id
	//st_vm wait_vm[VM_TOTAL]; //等待转移的虚拟机列表      包含各个维度，id   
	 //保留物理机列表，其中物理机资源的total为剩余值,包含各个维度，id（按照资源大小排列）
	//dec decision[VM_TOTAL]; //转移策略   包含虚拟机id，原本所在物理机id以及目标物理机id
	int k,l,x,m;  
	int off_pm;     //off_pm为保留物理机数量
	double vm_total,p_t=0; //vm_total为虚拟机资源总和，p_t为物理机资源总和
	int i;  /*i用于选择排序方式（已不用）*/
	int n_exm=_csthis->sd->pm_total; //putbackvm[ ]为存储待迁移虚拟机组id（其实应为putbackvmg[]）
	int no_off_pm;    //不能下电物理机数量

	//double hot=0.8,cold=0.2,warm=0.6; /*来自配置文件*/

	int vmgn,vmgn1;  int n_ret;
	
	

	 //vmgPtr vmg;

	int pmid;//用于保存取消下电物理机ID，add by sun in 8-16

	initSys(&st);
	st.sd=_csthis->sd;
	st.get_max_vec(dimMax);

	init_vmgroup (&vmg_cut);
	vmg_cut.sd=_csthis->sd;

		/*分组*/
	i=0; 
	_csthis->sd->vmg_total=0;
	while(i<_csthis->sd->pm_total){
		vmg_cut.grouping_in_pm(i);
		i++;
	}
	i=0;
	while(i<_csthis->sd->vmg_total){
		st.SumVMG(i);
		i++;
	}
	/*printf("请选择排序方式（1或2）：\n");//将选择排序方式删去，默认选择sort1，by sun in 8-16
	scanf("%d",&i);
	if(i==1)
		sort1 ( );
	else if(i==2)
		sort2 ( );*/
	sort2();

	//计算虚拟机总占用资源量
	vm_total=0;
	for(l=0;l<_csthis->sd->vm_total;l++)
		vm_total+=down_1dgree_tot(l, 'v');

	//计算所需物理机

	/*将其上有stat为2的虚拟机的物理机优先放入待迁入物理机列表*/
	no_off_pm=0;
	for(i=0;i<_csthis->sd->pm_total;i++)
		if( if_can_off(i,_csthis->sd->pmlist)==-1 ){
			wait_pm[no_off_pm]=&_csthis->sd->pmlist[i];
			no_off_pm++;
			p_t+=down_1dgree_tot(i,'p');
		}
	/*更新exm列表*/
	for(i=0;i<n_exm;i++)
		for(m=0;m<no_off_pm;m++)
			if(st.exm1[i].id==wait_pm[m]->host.id){
				for(k=i;k<n_exm-1;k++)
					st.exm1[k]=st.exm1[k+1];

				n_exm--;
				i--;
				break;
			}

	x=0;
	/*按负载能力 得到待迁入物理机列表 */
	while (p_t<vm_total&&x<n_exm){
		p_t=st.exm1[x].tot+p_t;
		wait_pm[no_off_pm++] = &_csthis->sd->pmlist[st.exm1[x].id];
		x++;
	}

	//x是st.exm[]队列中待下电物理机下标，no_off_pm是wait_pm[]队列的末指针


	/*将待下电物理机上的虚拟机组放入wait_vmg数组*/
	l=0;
	for(i=0;i<_csthis->sd->vmg_total;i++)
		for(m=x;m<n_exm;m++)
			if(st.exm1[m].id==_csthis->sd->vmglist[i].at_pmid){
				wait_vmg[l]=&_csthis->sd->vmglist[i];
				l++;
				break;
			}
	vmgn1=vmgn=l;

	/*存储虚拟机组的oldat信息*/
	//save_vmg_oldat(l, wait_vmg);
	
	n_ret=0;
	for (i = 0;i< _csthis->sd->pm_total;i++)
		_csthis->sd->pmlist[i].notColdPM = 0;
	st.placeVMGtoPM(wait_vmg,vmgn, wait_pm,no_off_pm, _csthis->sd->v2plist,&n_ret);

	while(n_ret==-1&&x<n_exm){
		//取回待下电物理机操作

		/*将第一台待下电取回*/

		wait_pm[no_off_pm++]=&_csthis->sd->pmlist[st.exm1[x++].id];
		wait_pm[no_off_pm-1]->notColdPM = 1;
		/*重新放置*/
		st.placeVMGtoPM(wait_vmg,vmgn, wait_pm,no_off_pm, _csthis->sd->v2plist,&n_ret);
	}

	/*输出决策*/
	/*输出下电物理机操作序列*/
	if (n_ret==-1){
		printf("Sorry, scheduling is failed.\n");
		return ;
	}
	for(i = x; i< n_exm; i ++){
		st.SetActionlist (ACTION_DOWN, &_csthis->sd->pmlist[st.exm1[i].id], NULL, NULL);
	}

	st.migrate_actionlist(wait_vmg, NULL, vmgn);

	/*
	m=0;
	//待下电物理机上虚拟机列表
	for(l=0;l<_csthis->sd->vm_total;l++)
		for(k=off_pm;k<_csthis->sd->pm_total;k++)
			if(_csthis->sd->vmlist[l].at==st.exm1[k].id){
				wait_vm[m]=_csthis->sd->vmlist[l];
				m++;
		}

	t_transfer_vm=m;   //t_transfer_vm为待迁移存储虚拟机总数
	*/

	/*
	//将保留物理机的大小更新为剩余大小
	for(l=0;l<off_pm;l++){
		for(k=0;k<DIM;k++){
			wait_pm[l].host.load[k].total
				=_csthis->sd->pmlist[st.exm1[l].id].host.load[k].total*hot
				-_csthis->sd->pmlist[st.exm1[l].id].host.load[k].used
				-_csthis->sd->pmlist[st.exm1[l].id].host.load[k].reserved;
		}
		wait_pm[l].host.id=st.exm1[l].id;
	}
	*/
	/*
	//ff放置虚拟机(已经成功放入l个)
	for(l=0;l<t_transfer_vm;l++){
		count=0;
		
		for(z=0;z<off_pm;z++){
			count=0;
			for(k=0;k<DIM;k++)
				if(wait_vm[l].host.load[k].total>wait_pm[z].host.load[k].total){
					count=1;break;
				}

			if(count==0){
				decision[l].id=wait_vm[l].host.id;
				decision[l].old_at=wait_vm[l].at;
				decision[l].now_to=wait_pm[z].host.id;
				for(k=0;k<DIM;k++)
					wait_pm[z].host.load[k].total=wait_pm[z].host.load[k].total-wait_vm[l].host.load[k].total;
				break;
			}
	}
	}
*/

	/*
	if(z==off_pm){//开机操作	
		for(k=0;k<DIM;k++)
			wait_pm[off_pm].host.load[k].total
			=_csthis->sd->pmlist[st.exm1[off_pm].id].host.load[k].total*hot
			-_csthis->sd->pmlist[st.exm1[off_pm].id].host.load[k].used
			-_csthis->sd->pmlist[st.exm1[off_pm].id].host.load[k].reserved;

		wait_pm[off_pm].host.id=st.exm1[off_pm].id;

		for(k=0;k<l;k++)
			if(decision[k].old_at==wait_pm[off_pm].host.id){
				for(z=0;z<off_pm;z++){
					if(wait_pm[z].host.id==decision[k].now_to)
						for(c=0;c<6;c++)
							wait_pm[z].host.load[c].total+=wait_vm[k].host.load[c].total;
				}

				for(z=0;z<t_transfer_vm;z++)
					if(wait_vm[z].host.id==decision[k].id){
						for(c=z;c<t_transfer_vm-1;c++)
							wait_vm[c]=wait_vm[c+1];
						break;
					}

					for(z=k;z<l-1;z++) decision[z]=decision[z+1];
			
					t_transfer_vm--;
					l--;
			}

			for(k=l;k<t_transfer_vm;k++)
				if(wait_vm[k].at==wait_pm[off_pm].host.id){
				for(c=k;c<t_transfer_vm-1;c++)
					wait_vm[c]=wait_vm[c+1];
				t_transfer_vm--;k--;
			}
		off_pm++;l--;
		}
*/
	/*
	printf("保留物理机数%d\n",off_pm);
	printf("转移虚拟机%d\n",m);
	printf("下电物理机");
	
	for(z=off_pm;z<_csthis->sd->pm_total;z++)
		printf("%d ",st.exm1[z].id);

	printf("\n保留物理机：");
	
	for(z=0;z<off_pm;z++)
		printf("%d ",st.exm1[z].id);

	printf("\n");
	
	for(k=0;k<t_transfer_vm;k++)
		printf("虚拟机编号：%d 原来所在物理机：%d 目标物理机：%d\n",
		decision[k].id,decision[k].old_at,decision[k].now_to);
		*/
}



