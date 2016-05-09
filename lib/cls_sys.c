#include "cls_sys.h"

#include <time.h>


/*��ʼ������*/
void initSys(clsstsysPtr THIS)
{
	sthis=THIS;

	THIS->getSysNonEmptyPM = getSysNonEmptyPM;

	THIS->getMinAvailCountOfVmg = getMinAvailCountOfVmg;

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

	THIS->moveVMGtoPM = moveVMGtoPM;
	THIS->moveVMtoPM = moveVMtoPM;

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
	THIS->sort = sort;

	THIS->random_shuffle = random_shuffle;

	THIS->SumVMG = SumVMG;

	THIS->migrate_actionlist = migrate_actionlist;



}

/*�Ѿ�����ĳ��vmg���õ�pm*/
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



/*�Ѿ�����ĳ��vm���õ�pm*/
void moveVMtoPM(vmPtr vm, pmPtr pm){
	int i;
	vm->at_pmid = pm->host.id;//vm->at ��Ϊ vm->at_pmid, vm->at_pmid��ʾǨ�ƺ����pmλ�ã� �ɵ�pm�Ծ���at
	for(i=0;i<DIM;i++)
		pm->host.load[i].used += vm->host.load[i].used;
	pm->p_vms [pm->countofvm++] = vm;
}

void sort(int *bg, int *ed, int (*cmp)(int a,int b)){
	int siz = ed-bg;
	int midval = *(bg+siz/2);
	int *i, *j, tmp;
	if (bg>=ed) return ;
	tmp = midval, *(bg+siz/2) = *bg, *bg = tmp;
	for (i = bg, j = ed-1; i<j; ){
		while(i<j && cmp(midval, *j)) j--;
		if (i<j) *(i++) = *j;
		while(i<j && cmp(*i, midval)) i++;
		if (i<j) *(j--) = *i;
	}
	*i = midval;
	sort(bg, i, cmp);
	sort(i+1, ed,cmp);
}



			

/*�ж�ĳ�����Ƿ���װ��ĳ�������*/
int can_be_packed(int part_no,int pm_no, vmgPtr ppart)
{
	int j;
	double remained,hot;
	cls_st_host sh;

	initHost(&sh);
	sh.sd = sthis->sd;
	sh.host = sthis->sd->pmlist[pm_no].host;

	

	/*�жϻ���ά���Ƿ��װ*/

	for(j=0;j<DIM;j++){
		//remained = sh.getAvail(j);/*��ȡʣ����Դ��С*/

		remained = sh.getTotal(j) * sthis->sd->hot_limit[j] 
		- sh.getUsed(j)-sh.getReserved(j);/*��ȡ�������ȵ��ʣ����Դ��С���޸���8-13��by sun*/
		//remained =_ithis->sd->pmlist[pm_no].load[j].total - _ithis->sd->pmlist[pm_no].load[j].reserved - _ithis->sd->pmlist[pm_no].load[j].used;
		if(ppart[part_no].load[j].total > remained) return FALSE;/*�ж��Ƿ��װ*/
	}

	return TRUE;
}

void charSwap(unsigned char *a, unsigned char *b){
	unsigned char tmp = *a;
	*a = *b;
	*b = tmp;
}

void memSwap(unsigned char *fst, unsigned char *sec, int len){
	int i;
	for (i = 0;i < len; i++)
		charSwap( fst+i, sec+i);
}

/*��һ��������������������ң���������ҿ���βָ�룬�Լ������͵��ֽ���*/
void random_shuffle(unsigned char *begin, unsigned char *end, int size){
	int cnt = 0, i, j, k;
	cnt = (end-begin)/size;
	for (i=0;i<cnt;i++){
		j = i+ rand()%(cnt-i);
		memSwap(begin+i*size, begin+j*size, size);
	}
	return ;

}

void insert_sg_afflist(vmgPtr a, int b, vm2vmPtr rel){
	memcpy(&sthis->sd->sg_afflist[sthis->sd->sg_afflist_total], 
		rel, sizeof(st_vm2vm));
	sthis->sd->sg_afflist[sthis->sd->sg_afflist_total].nextPtr = a->head;
	sthis->sd->sg_afflist[sthis->sd->sg_afflist_total].groupid = b;
	a->head = sthis->sd->sg_afflist_total;
	sthis->sd->sg_afflist_total++;

}


//�������� �ݹ����һ��vmg�����سɹ���ʧ��
int TryToPlace(int now,vm2pmPtr ret,int* n_ret, int lastPM){
	int i,j;
	int alldone = 0;
	int canPlace = 0;
	vm2vmPtr  rel;
	pmPtr pm;
	/*��ѡ���߼���
	 *(��̨PM�������������ڸ�VMG��kernel��� �� ��̨PM�������κ�����VMG����kernel)
	 *�����PM���׺����ܱ�֤
	 *������������
	 */
	vmgPtr vmg = queue_vmg[now];
	int isHot;
	if (now==queue_N){
		sthis->sd->lastPlacePM = lastPM;
		return 1;
	}
	if (now>sthis->maxSearchDepth)
		sthis->maxSearchDepth = now;
	if (vmg->vm_id_list[0]==3||vmg->vm_id_list[0]==5){
		lastPM = lastPM;
	}
	if (lastPM==-1){
		lastPM = sthis->sd->vmlist[vmg->vm_id_list[0]].at;

	}
	sthis->sd->singleEnd = clock();
	if ((sthis->sd->singleEnd-sthis->sd->singleStart)/1000>sthis->sd->singleTimeLimit)
		return 0;
	if (sthis->sd->DFScount >sthis->sd->singleDFScount)
		return 0;
	if (lastPM != -1){
		
		for (isHot = 0; isHot<2; isHot++){
			//���ȷ��õ�������η��ȵ���������ٴ�ѡ���ȵ������
			for ( i= 0 ;i< sthis->sd->pmlist[lastPM].nearPM_total; i++){
				pm = &sthis->sd->pmlist[sthis->sd->pmlist[lastPM].nearPM[i]];
				if (i>0){
					if (isHot==0&&pm->noNeedJudgeHot==1)
						continue;
					/*if (isHot==0&&pm->notColdPM ==1) //��һ�����ȷ��õ�������
						continue;*/
				}
					//can be placed
					if (pm->canBePlace&&
						(pm->countofpmgrp>vmg->is_kernel
						||(pm->countofpmgrp==0&& vmg->is_kernel==0))
						&&can_be_packed(0,pm->host.id,vmg)/*cjh modified hot 08.23*/
						&&checkPlaceAff(vmg, pm)){
							moveVMGtoPM(vmg,pm);
							ret[*n_ret].vm_id = vmg->groupid;
							ret[*n_ret].p_dst_pm = pm;
							(*n_ret)++;
							canPlace++;
				
							if (canPlace>availPMcount[now])
								availPMcount[now] = canPlace;
							if (TryToPlace(now+1,ret,n_ret, pm->host.id))							
								return 1;
				
							(*n_ret)--;
							getVMGoutofPM(vmg,pm);
					
				}
			}

		}
	}else //���û����һ̨���õ������������
	for (i= 0 ;i< sthis->n_pms; i++){
		//�����У����õ����������
		if ((sthis->pms[i]->countofpmgrp>vmg->is_kernel
			||(sthis->pms[i]->countofpmgrp==0&& vmg->is_kernel==0))
			&&can_be_packed(0,sthis->pms[i]->host.id,vmg)/*cjh modified hot 08.23*/
			&&checkPlaceAff(vmg, sthis->pms[i])){
				moveVMGtoPM(vmg,sthis->pms[i]);
				ret[*n_ret].vm_id = vmg->groupid;
				ret[*n_ret].p_dst_pm = sthis->pms[i];
				(*n_ret)++;
				canPlace++;
				
				if (canPlace>availPMcount[now])
					availPMcount[now] = canPlace;
				if (TryToPlace(now+1,ret,n_ret,sthis->pms[i]->host.id))
					return 1;
				
				(*n_ret)--;
				getVMGoutofPM(vmg,sthis->pms[i]);
		}
	}
		
	return 0;

}

void Generate_queue(vmgPtr src, vmgPtr *queue, int *N){
	//BFS ���ɴ������Ķ���
	int L = 0, j , k, t, id;
	vm2vmPtr  rel;
	vmPtr vm;
	queue[(*N)++] = src;
	inqueue[src->groupid] = 1;
	while(L<*N){
		src = queue[L++];
		for (j = 0; j< src->vm_total; j++){
			vm = &sthis->sd->vmlist[src->vm_id_list[j]];
			for (k = vm->head; k!=-1; k = sthis->sd->afflist[k].nextPtr){
				rel = &sthis->sd->afflist[k];
				if (rel->affinity<0){
					if (!inqueue[rel->p_dst_vm->at_vmgid]&&need_place[rel->p_dst_vm->at_vmgid]){
						inqueue[rel->p_dst_vm->at_vmgid] = 1;
						queue[(*N)++] = &sthis->sd->vmglist[rel->p_dst_vm->at_vmgid];
					}
				}else {
					for ( t = 0; t< rel->countofvm; t++)
					{
						id = rel->p_vms[t]->at_vmgid;
						if (!inqueue[id]&&need_place[id]){
							inqueue[id] = 1;
							queue[(*N)++] = &sthis->sd->vmglist[id];
						}
					}
				}
			}
		}
	}
	return ;


}

//n_ret����ǰ��Ҫ��ʼ��
/*��vmgroup���õ�pm�������׺��Եķ��÷���*/
int order[PM_TOTAL];
double maxLoadDim[DIM];
int loadCMP(int a,int b){
	int i;
	double sumA = 0, sumB = 0;
	for (i=0 ;i<DIM;i++)
	{
		sumA += (sthis->pms[a]->host.load[i].total-sthis->pms[a]->host.load[i].reserved-sthis->pms[a]->host.load[i].used)/maxLoadDim[i];
		sumB += (sthis->pms[b]->host.load[i].total-sthis->pms[b]->host.load[i].reserved-sthis->pms[b]->host.load[i].used)/maxLoadDim[i];
	}
	return sumA<sumB;
}
void placeVMGtoPM(vmgPtr *vmgs, int n_vmg, pmPtr *pms, int n_pms, 
				  vm2pmPtr ret,int* n_ret){
	
	int i,j,  k, t, u, vmid;
	int vmg_st_size;
	int failed;
	st_vm vm;
	vm2vmPtr  rel;
	pmPtr* tmp;
	int lastPM;

	sthis->n_pms = n_pms;
	sthis->pms = pms;
	for (i=0;i<sthis->sd->vmg_total;i++)
		need_place[i] = 0;
	for (i=0;i<n_vmg;i++)
		need_place[vmgs[i]->groupid] = 1;

	for ( i =0; i< n_vmg; i++){
		if (vmgs[i]->at_pmid!=INVI)
			getVMGoutofPM(vmgs[i], &sthis->sd->pmlist[vmgs[i]->at_pmid]);
	}
	for (i = 0; i< sthis->sd->pm_total; i++){
		sthis->sd->pmlist[i].canBePlace = 0;
	}
	for (i = 0; i< n_pms; i++) {
		sthis->sd->pmlist[pms[i]->host.id].canBePlace = 1;
	}

	
	//����ÿ����Ҫ��ͬ���ͬ��������
	sthis->sd->sg_afflist_total = 0;
	for ( i = 0;i<n_vmg;i ++)
		vmgs[i]->head = -1;
	for (i = 0;i < n_vmg; i++)
	{
		//�����Ҫͬ���vmg

		for (j=0; j< vmgs[i]->vm_total; j++)
		{
			vmid = vmgs[i]->vm_id_list[j];
			vm = sthis->sd->vmlist[vmid];
			for (k = vm.head ; k!=-1 ; k = sthis->sd->afflist[k].nextPtr){
				rel = &sthis->sd->afflist[k];
				if (rel->affinity == AFF_GR){
					for (t = 0; t< rel->countofvm; t++)
						insert_sg_afflist(vmgs[i], rel->p_vms[t]->at_vmgid,rel);
				}
			}
		}
	}



	//Ԥ�ȹ���ÿ��VMG�Ƿ�Ϊ����PM��Ľ���
	for (i = 0;i < n_vmg; i++)
	{
		//�����Ҫͬ���vmg
		for (j=0; j< sthis->sd->vmg_total;j++)
			same_group[j] = 0;
		for (j = vmgs[i]->head; j!=-1 ; j = sthis->sd->sg_afflist[j].nextPtr)
			same_group[sthis->sd->sg_afflist[j].groupid] = 1;

		//�����Ҫͬ���vmg�� ����Ҫ��ͬ��ķ��׺���
		for (j=0; j< sthis->sd->afflist_total; j++){
			rel = &sthis->sd->afflist[j];
			if (rel->affinity == N_AFF_GR){
				if (same_group[rel->p_src_vm->at_vmgid]
					&& same_group[rel->p_dst_vm->at_vmgid])
						break;
			}
		}
		if (j!=sthis->sd->afflist_total){
			is_kernel[i] = 1;
			vmgs[i]->is_kernel = 1;
		}else {
			is_kernel[i] = 0;
			vmgs[i]->is_kernel = 0;
		}
	}


	//��11111100000��˳������
	i = 0, j = n_vmg -1;
	while(i < j){
		//��һ��kernelΪ0���±� i
		while(is_kernel[i] == 1&&i<n_vmg) i++;

		//��һ��kernelΪ1���±� j
		while(is_kernel[j] == 0&&j>=0) j--;

		if (i<j){
			memSwap((unsigned char *)vmgs+sizeof(vmgPtr)*i, 
				(unsigned char *)vmgs+sizeof(vmgPtr)*j, sizeof(vmgPtr));
			i++, j--;
		}
	}

	//��������111111 �������
	random_shuffle((unsigned char *)vmgs, (unsigned char *)(vmgs+i),
		sizeof(vmgPtr));

	//��������00000 �������
	random_shuffle((unsigned char *)(vmgs+i), (unsigned char *)(vmgs+n_vmg), 
		sizeof(vmgPtr));

	//��������PM�б��������
	if (sthis->sd->isFirstPlace==0){
		sthis->sd->isFirstPlace = 1;
		get_max_vec(maxLoadDim);
		for (i= 0;i < n_pms; i++)
			order[i] = i;
		sort(order,order+n_pms,loadCMP);
		tmp = (pmPtr *)malloc(sizeof(pmPtr)*n_pms);
		for (i=0 ;i<n_pms;i++)
			tmp[i] = pms[order[i]];
		for (i=0 ;i<n_pms;i++)
			pms[i] = tmp[i];
		free(tmp);

	}else {
		random_shuffle((unsigned char *)pms, (unsigned char *)(pms+n_pms), 
			sizeof(pmPtr));
	}

	//����˳��1. �Ƿ�ΪPM�齻�� 2. �����굱ǰ�������ͬ��ͬ���

	for (i = 0; i< sthis->sd->vmg_total; i++)
		dfs_flag[i] = 0, inqueue[i] = 0;

	failed = 0;

	//���ڼ�¼���������и�VMG���ܷ��õ�PM��Ϊ���٣����ٵ��Ǹ�VMG��Ҫ���
	for (i = 0;i<n_vmg; i++)
		availPMcount[i] = 0;

	

	*n_ret = 0;
	
	if (sthis->sd->vmg_total<0){
		//������ȫͼ����
		queue_N = n_vmg;
		for (i=0 ;i< queue_N; i++)
			queue_vmg[i] = vmgs[i];
		sthis->maxSearchDepth = 0;
		if (queue_N>1){
			lastPM = pms[rand()%n_pms]->host.id;
		}else lastPM = -1;
		if (!TryToPlace(0,ret,n_ret,lastPM)){
			failed = 1;
		}
	}else {
		sthis->sd->lastPlacePM = -1;
		for (i=0;i<n_vmg; i++)
			if (inqueue[vmgs[i]->groupid]==0){
				queue_N = 0;
				Generate_queue(vmgs[i],queue_vmg, &queue_N);
				sthis->maxSearchDepth = 0;
				sthis->sd->DFScount = 0;
				sthis->sd->singleStart = clock();
				lastPM = -1;
				if (!TryToPlace(0, ret,n_ret, sthis->sd->lastPlacePM)) {
					failed = 1;
					break;
				}
				
			}
	}

	if (failed){
		for ( i =0; i< n_vmg; i++){
			if (vmgs[i]->at_pmid!=INVI)
				getVMGoutofPM(vmgs[i], &sthis->sd->pmlist[vmgs[i]->at_pmid]);
		}
		(*n_ret)  = -1;
	}

	return ;



}

int pmCountCmp(int a,int b){
	return availPMcount[a]<availPMcount[b];
}

int getMinAvailCountOfVmg(vmgPtr *p_vmgs,int n_vmg,int *order){
	int i;
	int cmpMin = 0x3fffffff, cmpI;

	for (i=0;i <n_vmg; i++)
		order[i] = i;
	sort(order,order+sthis->maxSearchDepth+1,&pmCountCmp);
	return 1;



}


/*���ĳ�η����Ƿ����㱻����pm�����ڵķ��׺���*/
int checkPlaceAff(vmgPtr vmg, pmPtr pm){
	int atpm;
	int i, vmid, j, k, u, yes, t;
	st_vm vm;
	vm2vmPtr  rel;
	int old_pm = vmg->at_pmid;
	if (old_pm!=-1){
		getVMGoutofPM(vmg,&sthis->sd->pmlist[old_pm]);
	}
	if (vmg->vm_total+pm->countofvm>=100)
		goto fail;
	moveVMGtoPM(vmg,pm);
	
	for (i=0;i<vmg->vm_total; i++){
		vmid = vmg->vm_id_list[i];
		vm = sthis->sd->vmlist[vmid];
		for (j = vm.head ; j!=-1 ; j = sthis->sd->afflist[j].nextPtr){
			rel = &sthis->sd->afflist[j];
			if (rel->affinity < 0){
				//���׺�
				//int atpm = rel->p_dst_vm->host.id;
				if(rel->p_dst_vm->at_pmid != -1)	//�޸�Ϊ���������Id��by sun in 8-15
					atpm = rel->p_dst_vm->at_pmid;
				else
					atpm = rel->p_dst_vm->at;

				//������ֻ�δ���ã�����
				if (atpm==-1) continue;

				if (rel->affinity==-1){
					if (atpm==pm->host.id) goto fail;
				}else if (rel->affinity==-2){
					if (sthis->sd->pmlist[atpm].at_p_fr->id
						== pm->at_p_fr->id)
						goto fail;
				}else if (rel->affinity==-3){
					if (pm->countofpmgrp==0||sthis->sd->pmlist[atpm].countofpmgrp==0){
						//anyone don't belong to any group
						if (sthis->sd->pmlist[atpm].host.id==pm->host.id)
							goto fail;
					
					}
					else 
					for (k = 0;k < pm->countofpmgrp;k ++)
						for (u = 0;u < sthis->sd->pmlist[atpm].countofpmgrp; u++)
							if (pm->at_p_pmgrps[k]->id == sthis->sd->pmlist[atpm].at_p_pmgrps[u]->id)
								goto fail;
				}
			}else {
				//�׺�
				if (rel->affinity_at!=-1){
					//�����ָ������λ�ã�����Ŀ��PM�Ƿ����
					//����޷����õ�ԭ��û�к����ʼ��affinity_at by CC 10.17
					if (rel->affinity==1){
						if (rel->affinity_at!=pm->host.id) goto fail;
					}else if (rel->affinity==2){
						if (rel->affinity_at!=pm->at_p_fr->id) goto fail;
					}else if (rel->affinity==3){
						if (pm->countofpmgrp==0)
							goto fail;
						for (k=0; k< pm->countofpmgrp;k++)
							if (pm->at_p_pmgrps[k]->id==rel->affinity_at)
								break;
						if (k==pm->countofpmgrp)
							goto fail;
					}
				}
				else {
					//��ָ��λ�ã�ö�ٸ��׺͹�ϵ�е�ÿһ̨�����
					for (t =0;t<rel->countofvm ;t++){
						int atpm = rel->p_vms[t]->at_pmid;

						//��δ���ã�����
						if (atpm==-1) continue;

						if (rel->affinity==1){
							if (atpm!=pm->host.id) goto fail;
						}else if (rel->affinity==2){
							if (sthis->sd->pmlist[atpm].at_p_fr->id
								!= pm->at_p_fr->id)
								goto fail;
						}else if (rel->affinity==3){
							yes = 0;
							if (pm->countofpmgrp==0||sthis->sd->pmlist[atpm].countofpmgrp==0){
								if (pm->host.id!=atpm)
									goto fail;
							}
							for (k = 0;k < pm->countofpmgrp;k ++)
								for (u = 0;u < sthis->sd->pmlist[atpm].countofpmgrp; u++)
									if (pm->at_p_pmgrps[k]->id== sthis->sd->pmlist[atpm].at_p_pmgrps[u]->id)
										yes = 1;
							if (!yes) goto fail;
						}
					}
				}
			}



		}
	}
	if (vmg->at_pmid!=-1)
		getVMGoutofPM(vmg, pm);
	if (old_pm!=-1){
		moveVMGtoPM(vmg,&sthis->sd->pmlist[old_pm]);
	}
	return 1;

fail:
	if (vmg->at_pmid!=-1)
		getVMGoutofPM(vmg,pm);
	if (old_pm!=-1){
		moveVMGtoPM(vmg,&sthis->sd->pmlist[old_pm]);
	}
	return 0;
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
		for(j=0;j<DIM;j++)
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
	double maxload [DIM];

	initHost(&sh);
	sh.sd = sthis->sd;
	tot = getSysNonEmptyPM();
	get_max_vec(maxload);

	if(tot==0)
		return 0.0;
	skw=0;i=0;
	while(i<DIM){
		//iά��ʹ����
		avg=getSysAvgUsed(i);
		if(avg!=0){
			
			for (j=0; j<sthis->sd->pm_total; j++){
				if (sthis->sd->pmlist[j].countofvm==0) continue;
				sh.host = sthis->sd->pmlist[j].host;
				temp = normalization(&sh.host, i, maxload) - avg;
				//printf("%d %d %lf\n",i,j,temp);
				skw += temp*temp*sthis->sd->weight[i];
				
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
	int tot;
	int i,j;
	double skw,avg,temp;
	cls_st_host sh;
	double maxload [DIM];
	if (pm->countofvm==0)
		return 0;
	initHost(&sh);
	sh.sd = sthis->sd;

	get_max_vec(maxload);


	skw=0;i=0;
	while(i<DIM){
		avg=getSysAvgUsed(i);
		if(avg!=0){			
			sh.host = pm->host;
			temp = normalization(&sh.host, i, maxload) - avg;
			skw += temp*temp*sthis->sd->weight[i];
				
		}
		i++;
	}
	return skw ;
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
PM ��ĳ��ά�ȵ�ʵ��ʹ��ֵռ�����İٷֱ�
VM ��ĳ��ά�ȵ�ʹ��ֵ��ռ����PM�����İٷֱ�
*/
double getPercent(hostPtr hptr, int name, int type)
{
	double down;
	vmPtr vm;
	pmPtr pm;
	cls_st_host shost;
	initHost(&shost);
	shost.sd=iHost->sd;
	shost.host=*hptr;

	if (type == T_VM) {//if hptr is a vm ? ���VM���뵽PM�к�ռPM���������İٷֱ�
		vm=&sthis->sd->vmlist[hptr->id];
		if (vm->host.stat == VMIGR){ // Not a init place vm�����״̬��Ǩ�ƣ�������VM��Ǩ��Ŀ���PM�еİٷֱ�
			pm=&sthis->sd->pmlist[vm->at_pmid];
			shost.host=pm->host;
			down = shost.getTotal(name);// get(pm.host.load, name, TOTAL);
		}
		else{
			down =shost.getTotal(name);// get(hptr->load, name, TOTAL);
		}
	} 
	else //��T_PM
		down =shost.getTotal(name);//  get(hptr->load,name, TOTAL);

	if (equal(down, 0))
		return 1;
	else
		//return (hptr->get(name, USED) + hptr->get(name, RESERVED)) / down;
		return (shost.getUsed(name)+ shost.getReserved(name)) / down;
}


/* 
��ȡĳ��ά�ȵ���ʹ����
*/
double getSysTotalUsed(int name)
{
	double totalUsed = 0;
	int i=0; int tot;
	cls_st_host sh;

	tot=sthis->sd->pm_total;

	initHost(&sh);
	sh.sd = sthis->sd;


	for (i = 0;i<tot;i ++){
		sh.host=sthis->sd->pmlist[i].host;
		if (sthis->sd->pmlist[i].countofvm==0||sh.getTotal(name)==0)
			continue;
		totalUsed +=(sh.getUsed(name)+sh.getReserved(name))/sh.getTotal(name);// sthis->sd->pmlist[i].host.load[name].used;
		
	}

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

int getSysNonEmptyPM(){
	int i;
	sthis->sd->sysNonEmptyPM = 0;
	for (i = 0 ;i < sthis->sd->pm_total; i++)
		if (sthis->sd->pmlist[i].countofvm>0)
			sthis->sd->sysNonEmptyPM ++;
	return sthis->sd->sysNonEmptyPM;
}
/* 
��ȡĳ��ά�ȵ�ƽ��ʹ����
*/
double getSysAvgUsed(int name)
{
	int tot;
	tot=getSysNonEmptyPM();

	if (tot == 0)
		return 0.0;
	else
		return getSysTotalUsed(name)/tot;
}

/*
* @�жϸ�������Ƿ�Ϊ�ȵ�
* ������ʽ�� pm->used+pm->reserved > pm->total*hot
*	�������ΪpmID���������Id
* ��Ϊ�ȵ㷵��True�����򷵻�False
*
* 2013.8.2 by sun
* ����һ������hot, ����ʱ���ⲿXML11���룬modified by cjh 08.21
*/
BOOL JudgeHot(int pmID,double hot)
{
	int d;
	double r;
	pmPtr pm;
	pm = &sthis->sd->pmlist[pmID];

	for (d=0; d< DIM ; d++){
		r=(pm->host.load[d].used + pm->host.load[d].reserved) / pm->host.load[d].total;
		if (r > sthis->sd->hot_limit[d]) 	return TRUE;
	}
	return FALSE;
}

/*
* @Ѱ���ȵ������
*	�������Ϊ&pmID��&load
* �ҵ�ռ������ߵ��������ά�ȣ���ֵ��ָ��
* ���ҵ��ȵ㷵��True�����򷵻�False
* 2013.8.2 by sun
*/
BOOL FindHotPm(int *pmID, int *load,double hot)
{
	int i, j;
	int hot_id =-1,hot_load=-1;
	pmPtr pm;
	pm = sthis->sd->pmlist;
	for(i=0;i<sthis->sd->pm_total;i++)
	if (!sthis->sd->pmlist[i].noNeedJudgeHot){
		if (hot_id==-1)
			hot_id = i,hot_load = 0;
		for(j=0;j<DIM;j++)
			if((pm[i].host.load[j].used / pm[i].host.load[j].total)>
				(pm[hot_id].host.load[hot_load].used / pm[hot_id].host.load[hot_load].total)){
					hot_id = i;
					hot_load = j;
			}
	}

	//���ȫ�����noNeedJudge
	
	*pmID = hot_id;
	*load = hot_load;
	if (hot_id==-1)
		return FALSE;
	if (JudgeHot(hot_id,hot))
		return TRUE;
	else
		return FALSE;
}

/*
* @�����ߴ���actionlist
*	�������Ϊaction{0��PLACE,1��MIGRATE,2��DOWN,3��UP}
* action	=0ʱ���贫��pm1, vm, ����ΪNull
*				=1ʱ���贫��pm1, pm2, vm
*				=2ʱ���贫��pm1, ����ΪNull
*				=3ʱ���贫��pm1, ����ΪNull
* ÿ��дһ�����߽���actionlist
* pm1Ϊԭ�������pm2ΪĿ�������
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
	
	}
	(*total)++;
}

/*�������ܣ���Ǩ�ƾ���д��actionlist
* ���������vmgָ��ҪǨ�Ƶ�vmg���ϵ�ָ��
    cls_old_at * old_atlist�� ԭ����vmg�����б�
	int n: vmg�ĸ���
�����ߣ��ֿ�ΰ
��ˣ�Jianhai Chen
*/

void migrate_actionlist(vmgPtr *vmg, cls_old_at * old_atlist, int n)
{
	int id,tot,old_at,new_at;
	
	int i,j;
	
	/*����ÿ��vmg*/
	for(i=0;i<n;i++){
		
		/*vmg��vm�ĸ���*/
		tot = vmg[i]->vm_total;
		
		/*ÿ��vm��Ǩ������һ��Ǩ�Ʋ���*/
		for(j=0;j<tot;j++)
		{
			id = vmg[i]->vm_id_list[j];/*��ȡvmid*/
			old_at = sthis->sd->vmlist[id].at;/*at������ǳ�ʼ����PM*/
			new_at = sthis->sd->vmlist[id].at_pmid;/*��ȡ�µķ���PM*/
			if (old_at==new_at)
				continue;
			/*���뵽actionlist*/
			SetActionlist(ACTION_MIGR, &sthis->sd->pmlist[old_at], 
				&sthis->sd->pmlist[new_at], &sthis->sd->vmlist[id]);
		}
	}
}

/*����:�������ȡ�������
������vm, Ҫ�Ƴ���vm��PM�������
�����ߣ�
�޸ģ�
*/
void getVMoutofPM(vmPtr vm, pmPtr pm){
	int i,n;
	n = pm->countofvm;

	/*����vm����pmΪ-1����ʾ��vm�Ƴ�PM*/
	vm->at_pmid = INVI;

	/*pm���ؼ�ȥ��ǰvm*/
	for(i=0;i<DIM;i++){
		pm->host.load[i].used -= vm->host.load[i].used;
		if (pm->host.load[i].used<0){
			i = i;
		}
	}
	
	
	/*����pm��vms����Ϊ��ȥ��VM*/
	for(i=0;i<n;i++){
		if(pm->p_vms[i]->host.id == vm->host.id){
			pm->p_vms[i] = pm->p_vms[n-1];/*����VM��ǰ*/
			pm->p_vms[n-1] = NULL;  /**/
			pm->countofvm--;
			break; 
		}
	}

}

/*���������ȡ�������*/
void getVMGoutofPM(vmgPtr vmg, pmPtr pm){
	int i;
	int vm_id;
	vmg->at_pmid = INVI;
	for(i=0;i<vmg->vm_total;i++){
		vm_id = vmg->vm_id_list[i];
		getVMoutofPM(&sthis->sd->vmlist[vm_id],pm);
	}
	return ;
}

/*�������������������������б�*/
void setVMLIST(vmPtr vm, vmgPtr vmg){

}

/* by jiajunwei  */

/*
* ������ : get_max_vec
* ���� : ���������ÿ��ά�����ֵ�������ָ��
* ������� : ��
* ����ֵ : double��ָ��
* @created by jia in 2013.8.3
*/
double* get_max_vec (double dim_max[]){
	int i, j;
	pmPtr ppm;
	
	/*��ʼ��*/
	for(i=0;i<DIM;i++)	dim_max [i]=1;
	
	/*����ÿ��PM���Ը���ά�������ֵ*/
	for(i=0;i<sthis->sd->pm_total;i++)
	{
		ppm=&sthis->sd->pmlist[i];

		for(j=0;j<DIM;j++)
			if(dim_max[j]<ppm->host.load[j].total)
				dim_max[j]=ppm->host.load[j].total;
		
	}
	return dim_max;
} 

/*
����ʹ����
*/
double normalization (hostPtr ptr, int dim, double max[ ]){
	if (ptr->load[dim].total==0) 
		return 0;
	return (ptr->load[dim].used+ptr->load[dim].reserved)/ptr->load[dim].total;
}

/*
* ������ : down_1dgree_tot
* ���� : ��������������������Դ�Ľ�ά���
* ������� : id������������������id���� 
	sign��'p'��ʾΪ�������'v'��ʾΪ�������
* ����ֵ : double��
* @created by jia in 2013.8.3
*/
double down_1dgree_tot (int id, char sign){
	double total=0,sum_vm,one_total;
	int k,d;

	hostPtr ptr1;
	

	
	/*�����PM*/
	if(sign=='p'){ 
		ptr1=&sthis->sd->pmlist[id].host;
		for(d=0;d<DIM;d++){
			total+=(ptr1->load[d].total-ptr1->load[d].reserved)/dimMax[d];
		}
	}

	/*�����VM*/
	if(sign=='v'){ 			
		ptr1=&sthis->sd->vmlist[id].host;
		for(d=0;d<DIM;d++){
			total+=ptr1->load[d].total/dimMax[d];
		}
	}

	return total;
}

/*
* ������ : down_1dgree_use
* ���� : �����������ʹ����Դ�Ľ�ά���
* ������� : id������������������id��
* ����ֵ : double��
* @created by jia in 2013.8.3
*/
double down_1dgree_use (int id){
	double total=0,vm_total,one_total;
	double dgree_max[DIM];
	int z,k;
	hostPtr vhost,phost;
	double* ptr=get_max_vec( dgree_max);
	hostPtr ptr1=&sthis->sd->pmlist[id].host;
	hostPtr ptr2=&sthis->sd->vmlist[id].host;
	vm_total=0;		

	for(z=0;z<DIM;z++){
		dgree_max[z]=*ptr;
		ptr++;  
	}

	for(z=0;z<sthis->sd->vm_total;z++){
		vhost=&sthis->sd->vmlist[z].host;
		for(k=0;k<DIM;k++)
			vm_total+=vhost->load[k].total/dgree_max[k];
	}

	for(z=0;z<DIM;z++){
		one_total=0;
		for(k=0;k<sthis->sd->vm_total;k++)
		{
			vhost=&sthis->sd->vmlist[k].host;
			one_total+=vhost->load[z].total/dgree_max[z];
		}
		total+=(ptr1->load[z].used/dgree_max [z])*one_total/vm_total;
	}

	return total;
}

/*
* ������ : sortbytotres
* ���� : ���������������Դ�Ӵ�С����
* ������� : ���ڴ洢������Ϣ������
* ����ֵ : ��
* @created by jia in 2013.8.3
*/
void sort1 ( ){
	int m,x,l;
	double total;
	sthis->exm1 [0].id=0;
	sthis->exm1 [0].tot=down_1dgree_tot (0, 'p');

	//�����������Դ������С�Ӵ�С����
	for(m=1;m<sthis->sd->pm_total;m++){
		total=0;	
		total=down_1dgree_tot (m, 'p');
		l=0;
		while(total<sthis->exm1 [l].tot){
			l++;
			if(l==m){l++;break;}
		}

		for(x=m;x>l;x--)
			sthis->exm1 [x]=sthis->exm1 [x-1];

		sthis->exm1 [l].id=m;
		sthis->exm1 [l].tot=total;
	}

}

/*
* ������ : sort2
* ���� : �������������ʹ����Դ�Ӵ�С����
* ������� : ���ڴ洢������Ϣ������
* ����ֵ : ��
* @created by jia in 2013.8.3
*/
void sort2 ( ){
	int m,x,l;
	double total_used,total;
	sthis->exm1 [0].id=0;
	sthis->exm1 [0].tot=down_1dgree_tot (0 ,'p');
	sthis->exm1 [0].total_used=down_1dgree_use(0);

	//��������ձ�ռ�����Ӵ�С����
	for(m=1;m<sthis->sd->pm_total;m++){
		total=total_used=0;
		l=0;

		total= down_1dgree_tot(m, 'p');
		total_used=down_1dgree_use(m);

		while(total_used<sthis->exm1 [l].total_used){
			l++;
			if(l==m) break;
		}

		for(x=m;x>l;x--)
			sthis->exm1 [x]=sthis->exm1 [x-1];

		sthis->exm1 [l].id=m;
		sthis->exm1 [l].tot=total;
		sthis->exm1 [l].total_used=total_used;
	}
}

/*
* ������ : get_sys_stat
* ���� : �ж�ϵͳ�����¶�
* ������� : ��
* ����ֵ : int��1��hot�� -1��cold�� 0��warm����
* @created by jia in 2013.8.5
*/
int get_sys_stat(double hot,double cold, double warm)
{
	double sum_vm=0,sum_pm=0,sum_pm1=0;
	//double hot=0.8,cold=0.2,warm=0.5;  //cancel by cjh  08.21
	int l,stat,flag=0;

	/*�����������Դ�ܺ�*/
	for(l=0;l<sthis->sd->vm_total;l++)
		sum_vm+=down_1dgree_tot(l, 'v');
	
	/*�����������Դ���ܺ�*/
	for(l=0;l<sthis->sd->pm_total;l++)
		sum_pm+=down_1dgree_tot(l,'p');
	
	/**/
	for(l=0;l<sthis->sd->pm_total;l++){
		sum_pm1+=down_1dgree_tot(l,'p')*warm;
		if(sum_pm1>sum_vm&& l!=sthis->sd->pm_total-1){
			flag=1;	break;
		}
	}
	if(sum_pm*hot<sum_vm) /*��ʾHOT*/
		stat=1;
	else if(flag==1)/*��ʾCOLD*/
		stat=-1;
	else
		stat=0;/*warm*/
	return stat;
}

//����������ɵ�at
//�����ò��ã�by sun in 8-16
void save_vmg_oldat(int n_vmg, vmgPtr vmg){
	/*int i;
	for(i=0;i<n_vmg;i++){
		sthis->old_atlist[i].id=vmg->groupid;
		sthis->old_atlist[i].old_at=vmg->at_pmid;
		vmg->at_pmid=-1;
		vmg++;
	}*/
}

/*��vmg��vm�ĸ��ؽ������*/
void SumVMG(int vmgid)
{
	int vmid,i,j;
	int tot;
	vmgPtr vmg;

	vmg = &sthis->sd->vmglist[vmgid];
	tot = sthis->sd->vmglist[vmgid].vm_total;
	
	for(i=0;i<tot;i++){
		vmid = vmg->vm_id_list[i];
		sthis->sd->vmlist[vmid].at_vmgid = vmgid;
		for(j=0;j<DIM;j++){
			if (i==0)
				vmg->load[j].total = 0;
			vmg->load[j].total += sthis->sd->vmlist[vmid].host.load[j].used;
			vmg->load[j].used =vmg->load[j].total;
		}
	}
	vmg->at_pmid = sthis->sd->vmlist[vmid].at;
}