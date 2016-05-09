/*
算法流程
	1、找出热点物理机，若有则取出合适的vmg放入待放置vmglist，重复，否则进入下一步
	2、调用placeVMGtoPM
	3、if（debug）打印结果
*/


/*
  Module: hotsolve module
  要求：输入获取负载数据，获取流量数据，获取PM时延矩阵
  输出：热点消除结果，一组迁移序列
  Created by chenjianhai.
  Created date: 2013/05/20
*/

#include "alg_hotsolve.h"

/*
@函数功能：热点消除算法类初始化
参数说明：THIS，指向类的指针
*/

void init_hotsolve(cls_hotsolvePtr THIS){//初始化类
	
	_hsthis=THIS;
	
	THIS->hotsolve=hotsolve;
	THIS->cold_hotsolve=cold_hotsolve;
	THIS->hot_hotsolve=hot_hotsolve;
	THIS->hot_pack=hot_pack;
	THIS->hot_store=hot_store;
	THIS->hot_update=hot_update;
	THIS->if_allhot=if_allhot;
//	THIS->if_onehot=if_onehot;
	THIS->output=output;
	//THIS->warm_hotsolve=warm_hotsolve;
//	THIS->transto_1D=transto_1D;
//	THIS->sort_pm=sort_pm;
	THIS->hot_pm=hot_pm;
	THIS->action_unpackvm=action_unpackvm;
	THIS->swap_pm=swap_pm;
}


/*初始化算法配置参数来自XML11,CJH ADD*/
void setRequest_hotsolve(cls_hotsolvePtr THIS)
{
	/*获取配置参数*/
	//THIS->hot=THIS->sd->req_hs->alg.para[0].para_value;
	//THIS->cold=THIS->sd->req_hs->alg.para[1].para_value;
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
	
		if (strcmp(p_req->type, "HOTSPOTSOLVE")==0){

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
					if (strcmp(p_req->alg.para[iPara].para_name,"maxTryTimes")==0){
						THIS->sd->maxTryTimesForHot = strtod(p_req->alg.para[iPara].para_str_val,NULL);
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

	/*待迁移虚拟机组列表指针*/
	vmgPtr vmg_list[WAIT_VMG_TOTAL];
	
	/*待迁移虚拟机列表*/
	vmgPtr wait_vmg_list[WAIT_VMG_TOTAL];

	pmPtr pmlist[PM_TOTAL];
/*
 * 函数名 : hotsolve
 * 功能 : 热点消除调度入口函数
 * 传入参数 : 无
 * 返回值 : 无
 * @created by 孙业成 in 2013.5.27
 */
void hotsolve()
{
	/*热点物理机及其最热维度*/
	int hot_id,hot_load;

	/*待迁移虚拟机组数*/
	int n_vmg;
	
	/*ret保存可迁移虚拟机组数，del_mig保存无法迁移虚拟机组数*/
	int ret,del_mig,del_pm=0;
	
	/*临时变量*/
	int i,j, temp,index;//临时变量

	/*临时变量，保存物理机ID*/
	int pm1,pm2;
	
	/*热点hot*/
	double hot;

	int tryTimes; //随机尝试的次数



	/*热点物理机ID信息*/
	int hot_pm[2000],id;
	/*热点物理机总数*/
	int hot_pm_tot;
	int pm_total;
	//st_vm overflow_vm;//无法放置的虚拟机负载和
	cls_st_sys css;
	cls_st_vm_group cvg;


	if(DEBUG) printf("Start to solve hot pm...\n");

	/*分组类初始化*/
	init_vmgroup(&cvg);
	cvg.sd = _hsthis->sd;
	_hsthis->sd->vmg_total=0;

	/*sys类初始化*/
	initSys(&css);
	css.sd = _hsthis->sd;
	
	//vmg_list = wait_vm_list;
	_hsthis->trans_num=0;/*迁移次数初始化*/
	n_vmg=0;/*待迁移虚拟机组数初始化*/

	hot=_hsthis->hot;
	pm_total = _hsthis->sd->pm_total;
	/*计算热点消除之前热点物理机数量，并输出*/
	for(i=hot_pm_tot=0;i<_hsthis->sd->pm_total;i++){
		if(css.JudgeHot(i,hot))
			hot_pm[hot_pm_tot++] = _hsthis->sd->pmlist[i].host.id;
	}
		
	if(DEBUG) printf("Before doing hotsolve, there are %d hot pms : \n", hot_pm_tot);
	/*输出所有的hotpm?*/	
	if(hot_pm_tot){
		for(i=0;i<hot_pm_tot-1;i++){
			if(DEBUG) printf("pm%d, ", hot_pm[i]);
			if(DEBUG) if(i==10)printf("\n");
		}
		if(DEBUG) printf("pm%d\n",hot_pm[i]);
	}
	

	/*对每台物理机上的虚拟机按照亲和性分组*/
	for(i=0;i<_hsthis->sd->pm_total;i++) cvg.grouping_in_pm(i);
	
	/*对每个虚拟机组内的负载加和*/
	for(i=0;i<_hsthis->sd->vmg_total;i++)	css.SumVMG(i);
	

	for (i=0;i<_hsthis->sd->pm_total;i ++){
		_hsthis->sd->pmlist[i].noNeedJudgeHot = 0;
		pmlist[i] = &_hsthis->sd->pmlist[i];
	}
	/*寻找热点，取出热点物理机上的虚拟机组，消除热点*/
	while(css.FindHotPm(&hot_id,&hot_load,hot)==TRUE){
		
		/*返回NULL表示该台物理机的热点无法消除*/
		for (i = 0;i<_hsthis->sd->vmg_total;i++)
			if (_hsthis->sd->vmglist[i].at_pmid==hot_id){
				for(j=0;j<_hsthis->sd->vmglist[i].vm_total;j++){//排除stat=2的虚拟机组，即保证虚拟机可迁移
					id = _hsthis->sd->vmglist[i].vm_id_list[j];
					if(_hsthis->sd->vmlist[id].host.stat==2)
						break;
				}
				if (j==_hsthis->sd->vmglist[i].vm_total){
					vmg_list[n_vmg] = &_hsthis->sd->vmglist[i];
					css.getVMGoutofPM(vmg_list[n_vmg], &_hsthis->sd->pmlist[hot_id]);
					n_vmg++;
				}

			}
		_hsthis->sd->pmlist[hot_id].noNeedJudgeHot = 1;

	}
	
	if(n_vmg!=0){		
		/*若无待迁移虚拟机组，则直接结束算法*/
		/*准备虚拟机组列表，用于调用放置算法*/
		
		del_mig = 0;		

		/*放置虚拟机组，若无法全部放置，则删除最后一组，
		重复执行，直到所有都无法放置*/
		ret = 0;
		tryTimes = 10;
		while(1){
			css.placeVMGtoPM(vmg_list, n_vmg-del_mig, 
				pmlist, pm_total, 
				_hsthis->sd->v2plist, &ret);

			if(ret!=-1){/*表示放置成功，保存迁移条数，跳出循环*/
				_hsthis->trans_num = ret;
				break;
			}
			else if (tryTimes==0){
				/*否则删去最后一组，再次尝试放置*/
				tryTimes = 10;
				del_mig ++;
				//将取出的最后一组 放置回原来的热点机
				css.moveVMGtoPM(vmg_list[n_vmg-del_mig], 
					&_hsthis->sd->pmlist[
						_hsthis->sd->vmlist[
							vmg_list[n_vmg-del_mig]->vm_id_list[0]].at]);
			} else tryTimes--;

			if(del_mig==n_vmg)/*若所有组均无法放置，则跳出循环*/
				break;
		}

		//migrate_actionlist(vmg_list[0], css.old_atlist, n_vmg-del_mig);

		for(i=index=0;i<n_vmg-del_mig;i++){//将决策放入actionlist
			temp = vmg_list[i]->vm_id_list[0];
			pm1 = vmg_list[i]->at_pmid;//dst_pm，目标物理机
			pm2 = _hsthis->sd->vmlist[temp].at;//src_pm，源物理机
			if(pm1 != pm2){//若目标物理机与源物理机不同，说明决策有意义，写入决策列表
				for(j=0;j<vmg_list[i]->vm_total;j++){
					temp = vmg_list[i]->vm_id_list[j];
					css.SetActionlist(1, &_hsthis->sd->pmlist[pm2], 
						&_hsthis->sd->pmlist[pm1], &_hsthis->sd->vmlist[temp]);//放置决策
					index++;
				}
			}
		}
		
		if(del_mig>0){//若有无法放置的虚拟机组，则将其信息也存入决策列表
			action_unpackvm(del_mig, vmg_list[n_vmg-del_mig]);
			if(DEBUG)
				printf("Sorry, scheduling is failed, because pms' total available resource isn't enough.\n");
		}

		if(DEBUG){//打印决策数量
			printf("There are %d actions.\n",index);
		}
	}
	else
		if(DEBUG)
			printf("Don't need migrate...\n");

	//计算热点消除后的热点物理机数量，并显示结果
	for(i=hot_pm_tot=0;i<_hsthis->sd->pm_total;i++){
		if(css.JudgeHot(i,hot))
			hot_pm[hot_pm_tot++] = _hsthis->sd->pmlist[i].host.id;
	}
	if(DEBUG) printf("After doing hotsolve, there are %d hot pms . \n", hot_pm_tot);
	if(hot_pm_tot){
		if(DEBUG) printf("Pmlist : ");
		for(i=0;i<hot_pm_tot-1;i++){
			if(DEBUG) printf("pm%d, ", hot_pm[i]);
			if(i==10)printf("\n");
		}
		if(DEBUG) ("pm%d\n", hot_pm[i]);
		if(DEBUG) printf("Sorry, scheduling is failed, because some pms have vms which cann't be migrated.\n");
	}
	
	return;
}

/* 
 * 函数名：selectVMG
 *  功能：取出物理机上合适的虚拟机组以消除热点
 * 传入参数：pm表示带消除热点的物理机，load表示带消除热点的维度
 * 返回值：若能消除热点，则返回虚拟机组地址；若不能消除，则返回NULL
 * create by sun 2013.8.4
 * modification : 增加了物理机上的虚拟机均不可迁移的判断
 * modify by sun 2013.8.16
 */
#define fabs(x) ((x)>0?(x):(-x))
st_vm_group *selectVMG(pmPtr pm, int load)
{
	int i,n,j,stat,id;//临时变量
	int vmg_id;//虚拟机组ID
	double hot;//hot值
	double skw = LONG_MAX;//距离恰好消除热点的偏移量
	int index = INVI;//判断该物理机上是否所有虚拟机组都不可迁移的标记
	double temp;//临时变量

	hot = HOT;
	n= pm->countofvm;
	for(i=0;i<n;i++){
		vmg_id = pm->p_vms[i]->at_vmgid;
		temp = _hsthis->sd->vmglist[vmg_id].load[load].used 
			- (pm->host.load[load].used - pm->host.load[load].total * hot);//计算偏移量，vmg.used>pm上超出热点的负载，则temp>0

		for(j=0,stat=1;j<_hsthis->sd->vmglist[vmg_id].vm_total;j++){//排除stat=2的虚拟机组，即保证虚拟机可迁移
			id = _hsthis->sd->vmglist[i].vm_id_list[j];
			if(_hsthis->sd->vmlist[id].host.stat==2)
				stat = 0;
		}
		if(fabs(temp)<skw&&stat==1){//找出可以刚好消除热点的vmg
			index = vmg_id;
			skw = fabs(temp);
		}
	}

	if(index!=-1)/*若index无值，则物理机上的虚拟机均不可迁移，返回Null表示错误，add by sun in 8-16*/
		return &_hsthis->sd->vmglist[index];
	else
		return NULL;
}

/*将无法放置的虚拟机列表输入actionlist*/
void action_unpackvm(int vmg_tot, vmgPtr vmg){
	actionPtr act;
	st_vm unpackvm_sum;//无法放置的虚拟机负载和
	vmPtr vm;
	int actionid, vmid, vm_tot;
	int i, j;//临时变量
	char msg[30];//临时变量

	actionid = _hsthis->sd->action_total++;//决策条数加一
	act = &_hsthis->sd->actionlist[actionid];
	act->action = 4;//action=4表示无法放置
	act->id = actionid;

	for(i=vm_tot=0;i<vmg_tot;i++){//将无法放置的虚拟机指针依次存入actionlist
		for(j=0;j<vmg[i].vm_total;j++){
			vmid = vmg[i].vm_id_list[j];
			act->unpackvm[vm_tot++] = &_hsthis->sd->vmlist[vmid];
		}
	}
	act->unpack_vm_total = vm_tot;//保存无法放置的虚拟机总数

	for(i=0;i<vm_tot;i++){//计算这些虚拟机的负载总和
		vm = act->unpackvm[i];
		if(!i){
			for(j=0;j<DIM;j++){
				strcpy(unpackvm_sum.host.load[j].metric_name, vm->host.load[j].metric_name);//给各个维度的名称初始化
				unpackvm_sum.host.load[j].used  = vm->host.load[j].used;
			}
		}else{
			for(j=0;j<DIM;j++){
				unpackvm_sum.host.load[j].used += vm->host.load[j].used;
			}
		}
	}
	//将信息存入mesg
	strcpy(act->mesg,"These VMs can not be migrated :");/*
	for(i=0;i<DIM;i++){
		sprintf(msg, " %s", unpackvm_sum.host.load[i].metric_name);
		strcat(act->mesg, msg);
		strcat(act->mesg, " = ");
		sprintf(msg, "%.2f", unpackvm_sum.host.load[i].used);
		strcat(act->mesg, msg);
		if(i!=7)
			strcat(act->mesg, ",");
	}*/
	act->msgtype = 2;//msgtype=2，表示是hotsolve算法产生的无法放置虚拟机列表
}

//交换两台物理机
void swap_pm(pmPtr pm1, pmPtr pm2)
{
	st_pm temp;
	memcpy(&temp, pm1, sizeof(st_pm));
	memcpy(pm1, pm2, sizeof(st_pm));
	memcpy(pm2, &temp, sizeof(st_pm));
}

/* | | | | | | | | | | | | | | | | | | | | | | | | | | | | |以下函数不用 | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | */

/*
 * 函数名 : transto_1D
 * 功能 : 降维公式函数
 * 传入参数 : 无
 * 返回值 : 无
 * @created by 孙业成 in 2013.5.27
 */
/*void transto_1D(void)
{
	int i,j;
	pmPtr p_pm;

	for(i=0;i<_hsthis->sd->pm_total;i++){
		p_pm=&_hsthis->sd->pmlist[i];
		_hsthis->pm_1d_list[i].id=i;
		for(j=0;j<DIM;j++)
			_hsthis->pm_1d_list[i].used 
				+= p_pm->host.load[j].used / p_pm->host.load[j].total;
	}
}*/

/*
 * 函数名 : sort_pm
 * 功能 : 物理机排序函数(用了选择排序，待改进)
 * 传入参数 : 无
 * 返回值 : 无
 * @created by 孙业成 in 2013.5.27
 */
/*void sort_pm(void)
{
	int i,j,index;
	st_pm_1D hot_pm;
	for(i=0;i<_hsthis->sd->pm_total;i++){
		for(index = j = i;  j < _hsthis->sd->pm_total; j++)
			if(_hsthis->pm_1d_list[j].used > _hsthis->pm_1d_list[index].used)
				index = j;
		hot_pm = _hsthis->pm_1d_list[i];
		_hsthis->pm_1d_list[i] = _hsthis->pm_1d_list[index];
		_hsthis->pm_1d_list[index] = hot_pm;
	}
}*/

/*
 * 函数名 : if_allhot
 * 功能 : 物理机整体状态判断函数
 * 传入参数 : 无
 * 返回值 : return 1：hot， return 0：warm， return -1：cold
 * @created by 孙业成 in 2013.5.27
 */
int if_allhot(void)
{
	st_pm hot__pm;
	st_vm hot__vm;
	int i,j,index;
	double warm;
	warm = (COLD+HOT)/2+(HOT-COLD)/4; /*最佳温度计算，CJH Labled*/
	for(i=0;i < _hsthis->sd->pm_total;i++)
		for(j=0;j<DIM;j++){
			if(!i)
				hot__pm.host.load[j].total = _hsthis->sd->pmlist[i].host.load[j].total * warm;
			else
				hot__pm.host.load[j].total += _hsthis->sd->pmlist[i].host.load[j].total * warm;
		}

	for(i=0;i<_hsthis->sd->vm_total;i++)
		for(j=0;j<DIM;j++){
			if(!i)
				hot__vm.host.load[j].used = _hsthis->sd->vmlist[i].host.load[j].used;
			else
				hot__vm.host.load[j].used +=  _hsthis->sd->vmlist[i].host.load[j].used;
		}

	for(i=0;i<DIM;i++)
		if(hot__vm.host.load[i].used>hot__pm.host.load[i].total*1.1) return HOTPM;
//		if(hot__vm.load[i].used<hot__pm.load[i].total*0.9)return -1;
	
	for(index=i=0;i<DIM;i++)
		if(hot__vm.host.load[i].used > hot__pm.host.load[i].total * 0.9) index++;
	
	if(!index)
		return COLDPM;
	else
		return WARMPM;
}

/*
 * 函数名 : if_onehot
 * 功能 : 物理机单个状态判断函数
 * 传入参数 : int pmID
 * 参数pmID : 待判断的物理机ID
 * 返回值 : return 1：hot， return 0：warm
 * @created by 孙业成 in 2013.5.27
 */
/*
int if_onehot(int pmID)
{
	int i;
	for(i=0;i<DIM;i++){
		if( (_hsthis->sd->pmlist[pmID].host.load[i].used / _hsthis->sd->pmlist[pmID].host.load[i].total) > HOT) return HOTPM;
	}
//	for(i=0;i<DIM;i++){
//
//		if((g_pmlist[pmID].load[i].used/g_pmlist[pmID].load[i].total)<0.2)return -1;
//	}

	return WARMPM;
}*/

/*
 * 函数名 : hot_hotsolve
 * 功能 : hot情况下hotsolve函数
 * 传入参数 : int pmID, int load_num
 * 参数pmID : 待消除热点的物理机
 * 参数load_num : 该物理机最热的维度
 * 返回值 : 无
 * @created by 孙业成 in 2013.5.27
 */
void hot_hotsolve(int pmID, int load_num)
{
	int i,index;
	_hsthis->hot_vm = 0;
	do{
		for(i=0,index=0;i<_hsthis->sd->vm_total;i++)
		{
			if(_hsthis->sd->vmlist[i].at==pmID)
				if(_hsthis->sd->vmlist[i].host.load[load_num].used > _hsthis->sd->vmlist[index].host.load[load_num].used)
					index=i;
		}
		_hsthis->vm_wait[_hsthis->hot_vm].id = index;
		_hsthis->vm_wait[_hsthis->hot_vm++].pm_src_id = _hsthis->sd->vmlist[index].at;
		_hsthis->sd->vmlist[index].at = -1;
		for(i=0;i<DIM;i++){
			_hsthis->sd->pmlist[pmID].host.load[i].used -= _hsthis->sd->vmlist[index].host.load[i].used;
			_hsthis->sd->pmlist[pmID].host.load[i].reserved = 
				_hsthis->sd->pmlist[pmID].host.load[i].total - _hsthis->sd->pmlist[pmID].host.load[i].used;
		}
		hot_pm(&pmID,&load_num);
//	}while(if_onehot(pmID));
	}while(0);

	for(i=0;i<_hsthis->hot_vm;i++){
		hot_pack(i);
		//printf("We need openning new computers\n");
	}
}

/*
 * 函数名 : warm_hotsolve
 * 功能 : warm情况下hotsolve函数
 * 传入参数 : int pmID, int load_num
 * 参数pmID : 待消除热点的物理机
 * 参数load_num : 该物理机最热的维度
 * 返回值 : 无
 * @created by 孙业成 in 2013.5.27
 */
/*warm情况下hotsolve函数*/
/*
void warm_hotsolve(int pmID, int load_num)
{
	cold_hotsolve(pmID,load_num);
}
*/

/*
 * 函数名 : cold_hotsolve
 * 功能 : cold情况下hotsolve函数
 * 传入参数 : int pmID, int load_num
 * 参数pmID : 待消除热点的物理机
 * 参数load_num : 该物理机最热的维度
 * 返回值 : 无
 * @created by 孙业成 in 2013.5.27
 */
void cold_hotsolve(int pmID, int load_num)
{
	int i,flag,index;
	int pm_dst_id;
	double used;
	used = 0;
	index = 0;
	for(i=0 ; i < _hsthis->sd->vm_total ; i++)
	{
		if(_hsthis->sd->vmglist[i].at_pmid==pmID)
			if ((used = _hsthis->sd->vmglist[i].load[load_num].used 
					/ _hsthis->sd->pmlist[pmID].host.load[load_num].total) > used)		//找出占用率最高的虚拟机组进行迁移
				index = i;
	}
	for(i=0,flag=0;i<_hsthis->sd->pm_total;i++)
	{
		flag = hot_update(pmID,_hsthis->pm_1d_list[i].id,index);
		if(flag)
			break;
	}
	pm_dst_id=i;
	if(!flag)
		hot_hotsolve(pmID,load_num);
	else
		hot_store(index,pmID,pm_dst_id);
}

/*
 * 函数名 : hot_store
 * 功能 : 将决策结果存入v2p数组
 * 传入参数 : int vmid, int pm_src_id, int pm_dst_id
 * 参数vmid : 迁移虚拟机ID
 * 参数pm_src_id : 源物理机
 * 参数pm_dst_id : 目标物理机
 * 返回值 : 无
 * @created by 孙业成 in 2013.5.27
 */
/*将结果存入v2p数组*/
void hot_store(int vmid, int pm_src_id, int pm_dst_id)
{
	int v2pid;
	v2pid=_hsthis->trans_num;
	_hsthis->sd->v2plist[v2pid].id=v2pid;
	_hsthis->sd->v2plist[v2pid].vm_id=vmid;
	_hsthis->sd->v2plist[v2pid].pm_src_id=pm_src_id;
	_hsthis->sd->v2plist[v2pid].pm_dst_id=pm_dst_id;
	_hsthis->trans_num++;
}

/*
 * 函数名 : output
 * 功能 : 决策结果函数
 * 传入参数 : 无
 * 返回值 : 无
 * @created by 孙业成 in 2013.5.28
 */
void output(void)
{
	int i;
	printf("vm2pm list:\n");
	if(!_hsthis->trans_num)
		printf("We don't need migrated.\n");
	else
		for(i=0;i<_hsthis->trans_num;i++)
			printf("v2pid = %4d , vmid = %4d , pm_src_id = %4d , pm_dst_id = %4d\n",
			_hsthis->sd->v2plist[i].id, _hsthis->sd->v2plist[i].vm_id, 
			_hsthis->sd->v2plist[i].pm_src_id, _hsthis->sd->v2plist[i].pm_dst_id);
}

/*
 * 函数名 : hot_update
 * 功能 : 更新源物理机和目标物理机负载函数
 * 传入参数 : int pm_src_id,int pmID,int vmID
 * 参数pm_src_id : 源物理机ID
 * 参数pmID : 目标物理机ID
 * 参数vmID : 虚拟机ID
 * 返回值 : return 1： 更新成功， return 0：更新失败
 * @created by 孙业成 in 2013.5.28
 */
int hot_update(int pm_src_id,int pmID,int vmID)
{
	int i;
	st_pm hot_pm;
	for(i=0;i<DIM;i++){
		hot_pm.host.load[i].used = _hsthis->sd->pmlist[pmID].host.load[i].used + _hsthis->sd->vmlist[vmID].host.load[i].used;
		if(hot_pm.host.load[i].used > _hsthis->sd->pmlist[pmID].host.load[i].total * HOT)
			return 0;
	}
	for(i=0;i<DIM;i++)
		_hsthis->sd->pmlist[pmID].host.load[i].used = hot_pm.host.load[i].used;
//		_hsthis->sd->pmlist[pmID].host.load[i].reserved = 
//			_hsthis->sd->pmlist[pmID].host.load[i].total - _hsthis->sd->pmlist[pmID].host.load[i].used;

	for(i=0;i<DIM;i++)
		_hsthis->sd->pmlist[pm_src_id].host.load[i].used -= _hsthis->sd->vmlist[vmID].host.load[i].used;
//		_hsthis->sd->pmlist[pm_src_id].host.load[i].reserved = 
//			_hsthis->sd->pmlist[pm_src_id].host.load[i].total - _hsthis->sd->pmlist[pm_src_id].host.load[i].used;
	
	return 1;
}

/*
 * 函数名 : hot_pack
 * 功能 : best fit装箱
 * 传入参数 : int vmid
 * 参数vmid : 待放置物理机在临时数据结构中的ID
 * 返回值 : 无
 * @created by 孙业成 in 2013.5.28
 */
/*装箱*/
void hot_pack(int vmid)
{
	int i,j;
	int packed,vmID,pm_no = -1;
	double remained;//剩余量
	double tmp,vol,max_vol = 0;
	vmID = _hsthis->vm_wait[vmid].id;
	while(pm_no == -1)
	{
		for(i=0;i<_hsthis->sd->pm_total;i++)//遍历所有物理机
		{
			packed = 1;
			for(j=0;j<8;j++) 
			{
				remained = _hsthis->sd->pmlist[i].host.load[j].total*HOT - _hsthis->sd->pmlist[i].host.load[j].used;
				packed = (packed && (_hsthis->sd->vmlist[vmID].host.load[j].used < remained));//判断是否可装
			}
			if(packed)//如果可装，计算VOL值
			{
				tmp = 1;
				for(j=0;j<8;j++) tmp *= 1 - _hsthis->sd->vmlist[vmID].host.load[j].used/(_hsthis->sd->pmlist[i].host.load[j].reserved);
				vol = 1 / tmp;
				if(vol > max_vol)//找出可装且VOL最大的虚拟机
				{
					max_vol = vol;
					pm_no = i;
				}
			}
		}
		if(pm_no != -1) //找到某一台物理机可装
		{
			_hsthis->sd->vmlist[vmID].at = pm_no;
			for(j=0;j<8;j++)
			{
				_hsthis->sd->pmlist[pm_no].host.load[j].used += _hsthis->sd->pmlist[vmID].host.load[j].used;//更新负载
				hot_store(vmID,_hsthis->vm_wait[vmid].pm_src_id,pm_no);
				break;
			}
		}
		else{		//无可装的物理机，则做继续新开物理机
			_hsthis->sd->pm_total++;
		}
	}
}



void hot_pm(int *id,int *load)
{
	int i,j;
	int hot_id,hot_load;
	for(i=0,hot_id=hot_load=0;i<_hsthis->sd->pm_total;i++){
		for(j=0;j<DIM;j++)
			if((_hsthis->sd->pmlist[i].host.load[j].used / _hsthis->sd->pmlist[i].host.load[j].total)>
				(_hsthis->sd->pmlist[hot_id].host.load[hot_load].used / _hsthis->sd->pmlist[hot_id].host.load[hot_load].total)){
				hot_id = i;
				hot_load = j;
			}
	}
	*id = hot_id;
	*load = hot_load;
}

