#include "cls_sys.h"



/*初始化对象*/
void initSys(clsstsysPtr THIS)
{
	sthis=THIS;
	
	

	/* Get the total used of the whole system with the specified metric */
	THIS->getSysTotalUsed=getSysTotalUsed;
	/* Get the average used of the whole system with the specified metric */

	THIS->getSysAvgUsed=getSysAvgUsed;
	/* Get the average used of the whole system with the specified metric */

	THIS->getSysTotalHave=getSysTotalHave;
	/* 
	* Calculate the current balance degree of the whole sys,
	* It's a common function for each algorithm
	*/

	THIS->getSysScore=getSysScore;

	THIS->getSysTotalLoad=getSysTotalLoad;

	//double getSysTotalLoad(AIMFP fp, const PMList &pmList);
	//double getSysAvgLoad(AIMFP fp, const PMList &pmList);
	//double getSysBalance(AIMFP fp, const PMList &pmList);

	THIS->getSysBalance = getSysBalance;

	THIS->getSysAvgLoad=getSysAvgLoad;

	/* 
	* Calculate one of the PM's several metric balance, 
	* to make sure of each the resouce be used nearly
	*/

	THIS->getPMSkw=getPMSkw;


	/* get the volume of the Host*/

	THIS->getVOL=getVOL;

	THIS->getVSR=getVSR;

	THIS->getAddVOL=getAddVOL;
	/* System state process functions */

	//THIS->getSysStat=getSysStat;

	THIS->setSysStat=setSysStat;	

	THIS->getSysStatRange=getSysStatRange;
	/* Get the resource usage percent*/
	THIS->getPercent=getPercent;

	THIS->getAddVOL=getAddVOL;

	THIS->JudgeHot=JudgeHot;

	THIS->FindHotPm=FindHotPm;

	THIS->SetActionlist=SetActionlist;

	THIS->getVMGoutofPM=getVMGoutofPM;

	THIS->getVMoutofPM=getVMoutofPM;

	THIS->get_max_vec=get_max_vec;
	
	THIS->down_1dgree_tot=down_1dgree_tot;
	
	THIS->normalization=normalization;
	
	THIS->down_1dgree_use=down_1dgree_use;
	
	THIS->get_sys_stat=get_sys_stat;
	
	THIS->sort1=sort1;
	
	THIS->sort2=sort2;

	THIS->save_vmg_oldat=save_vmg_oldat;

	THIS->can_be_packed = can_be_packed;
	THIS->checkPlaceAff = checkPlaceAff;
	THIS->placeVMGtoPM = placeVMGtoPM;
}

/*已决定将某个vmg放置到pm*/
void moveVMGtoPM(vmgPtr vmg, pmPtr pm){
	int n_vmg = vmg->vm_total;
	int i=0;
	int vm_id;
	while(i<n_vmg){
		vm_id = vmg->vm_id_list[i];
		moveVMtoPM(&sthis->sd->vmlist[vm_id],pm);
		i++;
	}
	vmg->at_pmid = pm->host.id;
}

/*已决定将某个vm放置到pm*/
void moveVMtoPM(vmPtr vm, pmPtr pm){
	int i;
	vm->at = pm->host.id;
	for(i=0;i<DIMENSION;i++)
		pm->host.load[i].used += vm->host.load[i].used;
	pm->p_vms [pm->countofvm++] = vm;
}

/*判断某个组是否能装入某个物理机*/
int can_be_packed(int part_no,int pm_no, vmgPtr ppart)
{

	int j;
	double remained;
	cls_st_host sh;
	
	initHost(&sh);
	sh.sd = sthis->sd;
	sh.host = sthis->sd->pmlist[pm_no].host;

	/*判断基本维度是否可装*/
	for(j=0;j<DIM;j++){
		remained = sh.getAvail(j);/*获取剩余资源大小*/
		//remained =_ithis->sd->pmlist[pm_no].load[j].total - _ithis->sd->pmlist[pm_no].load[j].reserved - _ithis->sd->pmlist[pm_no].load[j].used;
		if(ppart[part_no].load[j].total > remained) return FALSE;/*判断是否可装*/
	}

	return TRUE;
}

/*将vmgroup放置到pm，符合亲和性的放置方案*/
void placeVMGtoPM(vmgPtr vmgs, int n_vmg, pmPtr pms, int n_pms, vm2pmPtr ret,int* n_ret){
	//开发中 以下代码仅用于测试 采用First Fit方法
	int i,j;
	for (i=0;i<n_vmg;i++){
		for (j = 0; j < n_pms;j++)
			if (checkPlaceAff(&vmgs[i], &pms[j])){
				moveVMGtoPM(&vmgs[i], &pms[j]);
				
				break;
			}
		if (j==n_pms){
			*n_ret = -1;
			return ;
		}
		ret[i].vm_id = vmgs[i].groupid;
		ret[i].pm_dst_id = pms[j].host.id;
	}
	*n_ret = n_vmg;
	return ;
}



/*检查某次放置是否满足被放置pm框、组内的反亲和性*/
int checkPlaceAff(vmgPtr vmg, pmPtr pm){
	int i, vmid, j, k, u, yes, t;
	st_vm vm;
	vm2vmPtr  rel;
	for (i=0;i<vmg->vm_total; i++){
		vmid = vmg->vm_id_list[i];
		vm = sthis->sd->vmlist[vmid];
		for (j = vm.head ; j!=-1 ; j = sthis->sd->afflist[j].nextPtr){
			rel = &sthis->sd->afflist[j];
			if (rel->affinity < 0){
				//反亲和
				int atpm = rel->p_dst_vm->host.id;
				if (atpm==-1) continue;
				if (rel->affinity==-1){
					if (atpm==pm->host.id) return 0;
				}else if (rel->affinity==-2){
					if (sthis->sd->pmlist[atpm].at_fr_sid
						== pm->at_fr_sid)
						return 0;
				}else if (rel->affinity==-3){
					for (k = 0;k < pm->countofpmgrp;k ++)
						for (u = 0;u < sthis->sd->pmlist[atpm].countofpmgrp; u++)
							if (pm->at_gr_ids[k] == sthis->sd->pmlist[atpm].at_gr_ids[u])
								return 0;
				}
			}else {
				//亲和
				for (t =0;t<rel->countofvm ;t++){
					int atpm = rel->p_vms[t]->at_pmid;
					if (atpm==-1) continue;
					if (rel->affinity==2){
						if (sthis->sd->pmlist[atpm].at_p_fr->id
							!= pm->at_p_fr->id)
							return 0;
					}else if (rel->affinity==3){
						yes = 0;
						for (k = 0;k < pm->countofpmgrp;k ++)
							for (u = 0;u < sthis->sd->pmlist[atpm].countofpmgrp; u++)
								if (pm->at_gr_ids[k] == sthis->sd->pmlist[atpm].at_gr_ids[u])
									yes = 1;
						if (!yes) return 0;
					}
				}
			}


				
		}
	}
	
	return 1;
}


/* 
    * Calculate the current balance degree of the whole sys,
    * It's a common function for each algorithm
    */
double getSysScore(int flag)
{
	
	return 1.0;
}


double getSysTotalLoad()
{
	int i=0;
	int j;
	int total=sthis->sd->pm_total;
	double totalload=0;
	cls_st_host sh;
	
	initHost(&sh);
	sh.sd = sthis->sd;

	while(i<total){
		sh.host = sh.sd->pmlist[i].host;
		for(j=0;j<DIMENSION;j++)
			totalload += sh.getUsed(j);
		i++;
	}
	return totalload;
}

//double getSysTotalLoad(AIMFP fp, const PMList &pmList);
//double getSysAvgLoad(AIMFP fp, const PMList &pmList);
//double getSysBalance(AIMFP fp, const PMList &pmList);
double getSysBalance()
{
	int tot;
	int i,j;
	double skw,avg,temp;
	cls_st_host sh;
	double maxload [DIMENSION];

	initHost(&sh);
	sh.sd = sthis->sd;
	tot = sthis->sd->pm_total;
	get_max_vec(maxload);

	if(tot==0)
		return 0.0;
	skw=0;i=0;
	while(i<DIMENSION){
		avg=getSysAvgUsed(i);
		if(avg!=0){
			avg /= maxload[i];
			j=0;
			while(j<tot){
				sh.host = sthis->sd->pmlist[j].host;
				temp = normalization(&sh.host, i, maxload) - avg;
				skw += temp*temp;
				j++;
			}
		}
		i++;
	}
	return skw / tot;
}



double getSysAvgLoad()
{
	int tot;
	tot = sthis->sd->pm_total;
	if (tot==0)
		return 0.0;
	else
		return getSysTotalLoad()/tot;
}   
/* 
    * Calculate one of the PM's several metric balance, 
    * to make sure of each the resouce be used nearly
    */
double getPMSkw(pmPtr pm)
{
	return 0.0;
}


/* get the volume of the Host*/

/* 
 * @fn double Algorithm::getVOL(const HostPtr hptr)
 * use the formula to integrate 1/(1-cpu_percent)*(1-mem_percent) 
 * percent need to be in (0,1)
 * @param hptr the hptr need to be calculate metric used percent
 * @created by lix at 2011.8.13
 */
double getVOL(hostPtr hptr)
{
    double down = 1;
	int i;

    for (i = 0; i < DIM; i++)
         down *= 1 - getPercent(hptr, i,T_PM);

        //down *= log(1 - pow(getPercent(hptr, i),0.2));

    if (equal(down, 0))
        return 1<<30;
    else
        return 1/down;
}

/* 
 * @fn double Algorithm::getVSR(const HostPtr hptr)
 * need to devided by memory total 
 * percent need to be in (0,1)
 * @param hptr the hptr need to be calculate metric used percent
 * @created by lix at 2011.8.13
 */
double getVSR(pmPtr hptr)
{
	cls_st_host sh;
	
    double down; 
    double up;

	initHost(&sh);
	sh.sd=sthis->sd;

	down= sh.get(MEM, TOTAL);
	up  = getVOL(&hptr->host);
    if (equal(down, 0))
        return 1<<30;
    else
        return up / down;
}



double getAddVOL(pmPtr hptr){
	return 0.0;
}


/* System state process functions */
int getSysStat(){
	return 1;
}

int setSysStat(int stat){
	return 1;
}



/*
 * @fn int Algorithm::getSysStatRange()
 * Get the range of current system state
 * @return 1(hot state), 0(warm state) or -1(cold state)
 * @created by tuantuan at 2011.9.4
 */
int getSysStatRange(int range)
{
   // int range = (systemStat != 0) ? systemStat/abs(systemStat) : systemStat;

    /* Extra log messages */
    switch(range) {
        case 1:
            printf("The cluster is currently in hot state.\n");
            break;
        case 0:
            printf("The cluster is currently in warm state.\n");
            break;
        case -1:
            printf("The cluster is currently in cold state.\n");
            break;
        default:
            printf("The cluster is currently in unknown state.\n ");
            break;
    }

    return range;
}


/* 
 * @fn double Algorithm::getPercent(const HostPtr hptr, int name)
 * @param hptr need to be calculate metric used percent
 * @notes use rtti to judge dynamic class info, differ pm & vm
 * @when vm at is null, try to calculate total 
 * @created by lix at 2011.8.13
 PM 的某个维度的实际使用值占总量的百分比
 VM 的某个维度的使用值得占所在PM总量的百分比
 */
double getPercent(hostPtr hptr, int name, int type)
{
    double down;
	vmPtr vm;
	st_pm pm;
	cls_st_host shost;
	initHost(&shost);
	shost.sd=iHost->sd;
	shost.host=*hptr;

	if (type == T_VM) {//if hptr is a vm ? 求该VM在所在PM中的百分比
		vm=&sthis->sd->vmlist[hptr->id];
        if (vm->host.stat == VMIGR){ // Not a init place vm，如果状态是迁移，则计算该VM在迁移目标的PM中的百分比
        	pm=sthis->sd->pmlist[vm->at_pmid];
			shost.host=pm.host;
			down = shost.getTotal(name);// get(pm.host.load, name, TOTAL);
		}
        else{
            down =shost.getTotal(name);// get(hptr->load, name, TOTAL);
		}
    } 
	else //是T_PM
		down =shost.getTotal(name);//  get(hptr->load,name, TOTAL);

    if (equal(down, 0))
        return 1;
    else
        //return (hptr->get(name, USED) + hptr->get(name, RESERVED)) / down;
		return (shost.getUsed(name)+ shost.getReserved(name)) / down;
}


/* 
 * @fn double Algorithm::getSysTotalUsed(int name)
 * Get the total used of the whole system with the specified metric 
 * @param name metric name
 * @return the total used ..
 * @created by tuantuan at 2011.7.28
 */
double getSysTotalUsed(int name)
{
    double totalUsed = 0;
    int i=0; int tot;
	cls_st_host sh;
	
	tot=sthis->sd->pm_total;
   
	initHost(&sh);
	sh.sd = sthis->sd;

	
    while(i<tot){
		sh.host=sthis->sd->pmlist[i].host;
		totalUsed +=sh.getUsed(name);// sthis->sd->pmlist[i].host.load[name].used;
		i++;
    };

    return totalUsed;
}


/* 
 * @fn double Algorithm::getSysTotalHave(int name)
 * Get the total resource of the whole system with the specified metric 
 * @param name metric name
 * @return the total used ..
 * @created by lix at 2011.9.3
 */
double getSysTotalHave(int name)
{
    double totalHave = 0;

	int i=0; int tot;
	tot=sthis->sd->pm_total;
    
    while(i<tot){
		totalHave +=sthis->sd->pmlist[i].host.load[name].total;
		i++;
    };

    return totalHave;
}

/* 
 * @fn double Algorithm::getSysAvgUsed(int name)
 * Get the average used of the whole system with the specified metric 
 * @param name metric name
 * @return the average used ..
 * @created by tuantuan at 2013.5.31
 */
double getSysAvgUsed(int name)
{
     int tot;
	tot=sthis->sd->pm_total;

    if (tot == 0)
        return 0.0;
    else
        return getSysTotalUsed(name)/tot;
}

/*
 * @判断该物理机是否为热点
 *	传入参数为pmID，即物理机Id
 * 若为热点返回True，否则返回False
 * 2013.8.2 by sun
 */
BOOL JudgeHot(int pmID)
{
	int i;
	double hot;
	pmPtr pm;
	pm = sthis->sd->pmlist;
	hot = 0.8;
	for(i=0;i<10;i++){
		if(sthis->sd->reqlist->alg.para[i].para_name == "HOT"){
			hot = sthis->sd->reqlist->alg.para[i].para_value;
			break;
		}
	}
	for (i=0 ; i < DIMENSION ; i++)
		if (( pm[pmID].host.load[i].used / pm[pmID].host.load[i].total) > hot )
			return TRUE;
	return FALSE;
}

/*
 * @寻找热点物理机
 *	传入参数为&pmID，&load
 * 找到占用率最高的物理机的维度，赋值给指针
 * 若找到热点返回True，否则返回False
 * 2013.8.2 by sun
 */
BOOL FindHotPm(int *pmID, int *load)
{
	int i, j;
	int hot_id,hot_load;
	pmPtr pm;
	pm = sthis->sd->pmlist;
	for(i=hot_id=hot_load=0;i<sthis->sd->pm_total;i++){
		for(j=0;j<DIMENSION;j++)
			if((pm[i].host.load[j].used / pm[i].host.load[j].total)>
				(pm[hot_id].host.load[hot_load].used / pm[hot_id].host.load[hot_load].total)){
				hot_id = i;
				hot_load = j;
			}
	}
	*pmID = hot_id;
	*load = hot_load;
	if (JudgeHot(hot_id))
		return TRUE;
	else
		return FALSE;
}

/*
 * @将决策存入actionlist
 *	传入参数为action{0：PLACE,1：MIGRATE,2：DOWN,3：UP}
 * action	=0时，需传入pm1, vm, 其余为Null
 *				=1时，需传入pm1, pm2, vm
 *				=2时，需传入pm1, 其余为Null
 *				=3时，需传入pm1, 其余为Null
 * 每次写一条决策进入actionlist
 * pm1为原物理机，pm2为目标物理机
 * 2013.8.2 by sun
 */
void SetActionlist (int action, pmPtr pm1, pmPtr pm2, vmPtr vm)
{
	int *total;
	actionPtr act;
	act = sthis->sd->actionlist;
	total = &sthis->sd->action_total;
	act[*total].id = *total;
	act[*total].action=action;
	switch(action){
	case 0:	//PLACE
		act[*total].place_vm = vm;
		act[*total].place_pm = pm1;
		break;
	case 1:	//MIGRATE
		act[*total].mig_src = pm1;
		act[*total].mig_dst = pm2;
		act[*total].mig_vm = vm;
		break;
	case 2:	//DOWN
		act[*total].down_pm = pm1;
		break;
<<<<<<< .mine
	case 3:	//UP
		//act[*total].up_pm = pm1;
		//act[*total].up_num++;
		break;
=======

>>>>>>> .r287
	}
	*total++;
}

/*将迁移决策写入actionlist*/
void migrate_actionlist(vmgPtr vmg, cls_old_at * old_atlist, int n){
	int id ;
	int old_at;
	int new_at;
	int i;
	int tot;
	for(i=tot=0;i<n;i++){
		tot += vmg->vm_total;
	}
	for(i=0;i<tot;i++){
		id = old_atlist->id;
		old_at = old_atlist->old_at;
		new_at = sthis->sd->vmlist[id].at;
		SetActionlist(1, &sthis->sd->pmlist[old_at], &sthis->sd->pmlist[new_at], &sthis->sd->vmlist[id]);
	}
}

/*将虚拟机取出物理机*/
void getVMoutofPM(vmPtr vm, pmPtr pm){
	int i,n;
	n = pm->countofvm;
	//vm->at = INVI;
	//vm->at_pmid = INVI;
	for(i=0;i<DIMENSION;i++){
		pm->host.load[i].used -= vm->host.load[i].used;
	}
	for(i=0;i<n;i++)
		if(pm->p_vms[i]->host.id == vm->host.id){
			pm->p_vms[i] = pm->p_vms[n];
			pm->p_vms[n] = NULL;
			pm->countofvm--;
			break; 
		}
		return;
}

/*将虚拟机组取出物理机*/
void getVMGoutofPM(vmgPtr vmg, pmPtr pm){
	int i;
	int vm_id;
	//vmg->at_pmid = INVI;
	for(i=0;i<vmg->vm_total;i++){
		vm_id = vmg->vm_id_list[i];
		getVMoutofPM(&sthis->sd->vmlist[vm_id],pm);
	}
	return ;
}

/*将虚拟机组放入带放置虚拟机组列表*/
void setVMLIST(vmPtr vm, vmgPtr vmg){
}

/* by jiajunwei  */

/*
* 函数名 : get_max_vec
 * 功能 : 返回物理机每个维度最大值的数组的指针
 * 传入参数 : 无
 * 返回值 : double型指针
 * @created by jia in 2013.8.3
 */
double* get_max_vec (double dgree_max[]){
	int i, j;
	for(i=0;i<DIMENSION;i++)
		dgree_max [i]=0;
	for(i=0;i<sthis->sd->pm_total;i++)
		for(j=0;j<DIMENSION;j++)
			if(dgree_max [j]<sthis->sd->pmlist[i].host.load[j].total)
				dgree_max [j]=sthis->sd->pmlist[i].host.load[j].total;
	return dgree_max;
} 

/*
* 函数名 : normalization
 * 功能 : 返回归一化结果
 * 传入参数 : ptr（样例：&sthis->sd->pmlist[id].host），dgree（维度），max[ ]（各维度最大值数组）
 * 返回值 : double型
 * @created by jia in 2013.8.3
 */
double normalization (st_host * ptr, int dgree, double max[ ]){
	return ptr->load[dgree].total/max[dgree];
}

/*
* 函数名 : down_1dgree_tot
 * 功能 : 返回物理机或虚拟机总资源的降维结果
 * 传入参数 : id（物理机或是虚拟机的id）， sign（'p'表示为物理机，'v'表示为虚拟机）
 * 返回值 : double型
 * @created by jia in 2013.8.3
 */
double down_1dgree_tot (int id, char sign){
	double total=0,vm_total,one_total;
	double dgree_max[DIMENSION];
	int z,k;
	double* ptr=get_max_vec(dgree_max);
	st_host * ptr1;
	st_host * ptr2=&sthis->sd->vmlist[id].host;
	vm_total=0;		
	
		for(z=0;z<DIMENSION;z++){
			dgree_max[z]=*ptr;
			ptr++;
		}
		for(z=0;z<sthis->sd->vm_total;z++)
			for(k=0;k<DIMENSION;k++)
				vm_total=vm_total+sthis->sd->vmlist[z].host.load[k].total/dgree_max[k];

		if(sign=='p'){ 
			ptr1=&sthis->sd->pmlist[id].host;
			for(z=0;z<DIMENSION;z++){
				one_total=0;
				for(k=0;k<sthis->sd->vm_total;k++)
					one_total=one_total+sthis->sd->vmlist[k].host.load[z].total/dgree_max[z];
				total=normalization (ptr1, z, dgree_max)*one_total/vm_total+total;
			}
		}

		if(sign=='v'){ 			
			ptr1=&sthis->sd->vmlist[id].host;
			for(z=0;z<DIMENSION;z++){
				one_total=0;
				for(k=0;k<sthis->sd->vm_total;k++)
					one_total=one_total+sthis->sd->vmlist[k].host.load[z].total/dgree_max[z];
				total=normalization (ptr1, z, dgree_max)*one_total/vm_total+total;
			}
		}

		return total;
}

/*
* 函数名 : down_1dgree_use
 * 功能 : 返回物理机在使用资源的降维结果
 * 传入参数 : id（物理机或是虚拟机的id）
 * 返回值 : double型
 * @created by jia in 2013.8.3
 */
double down_1dgree_use (int id){
	double total=0,vm_total,one_total;
	double dgree_max[DIMENSION];
	int z,k;
	double* ptr=get_max_vec( dgree_max);
	st_host * ptr1=&sthis->sd->pmlist[id].host;
	st_host * ptr2=&sthis->sd->vmlist[id].host;
	vm_total=0;		

		for(z=0;z<DIMENSION;z++){
			dgree_max[z]=*ptr;
			ptr++;
		}

		for(z=0;z<sthis->sd->vm_total;z++)
			for(k=0;k<DIMENSION;k++)
				vm_total=vm_total+sthis->sd->vmlist[z].host.load[k].total/dgree_max[k];

		for(z=0;z<DIMENSION;z++){
			one_total=0;
			for(k=0;k<sthis->sd->vm_total;k++)
				one_total=one_total+sthis->sd->vmlist[k].host.load[z].total/dgree_max[z];
			total=(ptr1->load[z].used/dgree_max [z])*one_total/vm_total+total;
		}
		return total;
}

/*
* 函数名 : sort1
 * 功能 : 将物理机按照总资源从大到小排序
 * 传入参数 : 用于存储排序信息的数组
 * 返回值 : 无
 * @created by jia in 2013.8.3
 */
void sort1 ( ){
	int m,x,l;
	double total;
	sthis->exm1 [0].id=0;
	sthis->exm1 [0].tot=down_1dgree_tot (0, 'p');

		//物理机按照资源总量大小从大到小排序
		for(m=1;m<sthis->sd->pm_total;m++){
			total=0;	
			total=down_1dgree_tot (m, 'p');
			l=0;
			while(total<sthis->exm1 [l].tot){
				l++;
				if(l==m){
					l++;break;
				}
			}
			for(x=m;x>l;x--)
				sthis->exm1 [x]=sthis->exm1 [x-1];

			sthis->exm1 [l].id=m;
			sthis->exm1 [l].tot=total;
		}

}

/*
* 函数名 : sort2
 * 功能 : 将物理机按照已使用资源从大到小排序
 * 传入参数 : 用于存储排序信息的数组
 * 返回值 : 无
 * @created by jia in 2013.8.3
 */
void sort2 ( ){
	int m,x,l;
	double total_used,total;
	sthis->exm1 [0].id=0;
	sthis->exm1 [0].tot=down_1dgree_tot (0 ,'p');
	sthis->exm1 [0].total_used=down_1dgree_use(0);

		//物理机按照被占用量从大到小排序
		for(m=1;m<sthis->sd->pm_total;m++){
			total=total_used=0;
			l=0;

			total= down_1dgree_tot(m, 'p');
			total_used=down_1dgree_use(m);

			while(total_used<sthis->exm1 [l].total_used){
				l++;
				if(l==m){
					break;
				}

			}

			for(x=m;x>l;x--)
				sthis->exm1 [x]=sthis->exm1 [x-1];

			sthis->exm1 [l].id=m;
			sthis->exm1 [l].tot=total;
			sthis->exm1 [l].total_used=total_used;
		}
	}

/*
* 函数名 : get_sys_stat
 * 功能 : 判断系统总体温度
 * 传入参数 : 无
 * 返回值 : int（1：hot； -1：cold； 0：warm；）
 * @created by jia in 2013.8.5
 */
int get_sys_stat(){
	int vm_total=0,pm_total=0,pm_total1=0;
	double hot=0.8,cold=0.2,warm=0.5;
	int l,stat,flag=0;
	//计算虚拟机资源总和
	for(l=0;l<sthis->sd->vm_total;l++)
		vm_total+=down_1dgree_tot(l, 'v');
	
	for(l=0;l<sthis->sd->pm_total;l++)
		pm_total+=down_1dgree_tot(l,'p');

	for(l=0;l<sthis->sd->pm_total;l++){
		pm_total1=pm_total1+down_1dgree_tot(l,'p')*warm;
		if(pm_total1>vm_total&&l!=sthis->sd->pm_total-1){
			flag=1;
			break;
		}
	}
	if(pm_total*hot<vm_total)
		stat=1;
	else if(flag==1)
		stat=-1;
	else
		stat=0;
	return stat;
}


void save_vmg_oldat(int n_vmg, vmgPtr vmg){
	int i;
	for(i=0;i<n_vmg;i++){
		sthis->old_atlist[i].id=vmg->groupid;
		sthis->old_atlist[i].old_at=vmg->at_pmid;
		vmg->at_pmid=-1;
		vmg++;
	}
}