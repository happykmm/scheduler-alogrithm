/*
�㷨����
	1���ҳ��ȵ��������������ȡ�����ʵ�vmg���������vmglist���ظ������������һ��
	2������placeVMGtoPM
	3��if��debug����ӡ���
*/


/*
  Module: hotsolve module
  Ҫ�������ȡ�������ݣ���ȡ�������ݣ���ȡPMʱ�Ӿ���
  ������ȵ����������һ��Ǩ������
  Created by chenjianhai.
  Created date: 2013/05/20
*/

#include "alg_hotsolve.h"

/*
@�������ܣ��ȵ������㷨���ʼ��
����˵����THIS��ָ�����ָ��
*/

void init_hotsolve(cls_hotsolvePtr THIS){//��ʼ����
	
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


/*��ʼ���㷨���ò�������XML11,CJH ADD*/
void setRequest_hotsolve(cls_hotsolvePtr THIS)
{
	/*��ȡ���ò���*/
	//THIS->hot=THIS->sd->req_hs->alg.para[0].para_value;
	//THIS->cold=THIS->sd->req_hs->alg.para[1].para_value;
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

	/*��Ǩ����������б�ָ��*/
	vmgPtr vmg_list[WAIT_VMG_TOTAL];
	
	/*��Ǩ��������б�*/
	vmgPtr wait_vmg_list[WAIT_VMG_TOTAL];

	pmPtr pmlist[PM_TOTAL];
/*
 * ������ : hotsolve
 * ���� : �ȵ�����������ں���
 * ������� : ��
 * ����ֵ : ��
 * @created by ��ҵ�� in 2013.5.27
 */
void hotsolve()
{
	/*�ȵ��������������ά��*/
	int hot_id,hot_load;

	/*��Ǩ�����������*/
	int n_vmg;
	
	/*ret�����Ǩ�������������del_mig�����޷�Ǩ�����������*/
	int ret,del_mig,del_pm=0;
	
	/*��ʱ����*/
	int i,j, temp,index;//��ʱ����

	/*��ʱ���������������ID*/
	int pm1,pm2;
	
	/*�ȵ�hot*/
	double hot;

	int tryTimes; //������ԵĴ���



	/*�ȵ������ID��Ϣ*/
	int hot_pm[2000],id;
	/*�ȵ����������*/
	int hot_pm_tot;
	int pm_total;
	//st_vm overflow_vm;//�޷����õ���������غ�
	cls_st_sys css;
	cls_st_vm_group cvg;


	if(DEBUG) printf("Start to solve hot pm...\n");

	/*�������ʼ��*/
	init_vmgroup(&cvg);
	cvg.sd = _hsthis->sd;
	_hsthis->sd->vmg_total=0;

	/*sys���ʼ��*/
	initSys(&css);
	css.sd = _hsthis->sd;
	
	//vmg_list = wait_vm_list;
	_hsthis->trans_num=0;/*Ǩ�ƴ�����ʼ��*/
	n_vmg=0;/*��Ǩ�������������ʼ��*/

	hot=_hsthis->hot;
	pm_total = _hsthis->sd->pm_total;
	/*�����ȵ�����֮ǰ�ȵ�����������������*/
	for(i=hot_pm_tot=0;i<_hsthis->sd->pm_total;i++){
		if(css.JudgeHot(i,hot))
			hot_pm[hot_pm_tot++] = _hsthis->sd->pmlist[i].host.id;
	}
		
	if(DEBUG) printf("Before doing hotsolve, there are %d hot pms : \n", hot_pm_tot);
	/*������е�hotpm?*/	
	if(hot_pm_tot){
		for(i=0;i<hot_pm_tot-1;i++){
			if(DEBUG) printf("pm%d, ", hot_pm[i]);
			if(DEBUG) if(i==10)printf("\n");
		}
		if(DEBUG) printf("pm%d\n",hot_pm[i]);
	}
	

	/*��ÿ̨������ϵ�����������׺��Է���*/
	for(i=0;i<_hsthis->sd->pm_total;i++) cvg.grouping_in_pm(i);
	
	/*��ÿ����������ڵĸ��ؼӺ�*/
	for(i=0;i<_hsthis->sd->vmg_total;i++)	css.SumVMG(i);
	

	for (i=0;i<_hsthis->sd->pm_total;i ++){
		_hsthis->sd->pmlist[i].noNeedJudgeHot = 0;
		pmlist[i] = &_hsthis->sd->pmlist[i];
	}
	/*Ѱ���ȵ㣬ȡ���ȵ�������ϵ�������飬�����ȵ�*/
	while(css.FindHotPm(&hot_id,&hot_load,hot)==TRUE){
		
		/*����NULL��ʾ��̨��������ȵ��޷�����*/
		for (i = 0;i<_hsthis->sd->vmg_total;i++)
			if (_hsthis->sd->vmglist[i].at_pmid==hot_id){
				for(j=0;j<_hsthis->sd->vmglist[i].vm_total;j++){//�ų�stat=2��������飬����֤�������Ǩ��
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
		/*���޴�Ǩ��������飬��ֱ�ӽ����㷨*/
		/*׼����������б����ڵ��÷����㷨*/
		
		del_mig = 0;		

		/*����������飬���޷�ȫ�����ã���ɾ�����һ�飬
		�ظ�ִ�У�ֱ�����ж��޷�����*/
		ret = 0;
		tryTimes = 10;
		while(1){
			css.placeVMGtoPM(vmg_list, n_vmg-del_mig, 
				pmlist, pm_total, 
				_hsthis->sd->v2plist, &ret);

			if(ret!=-1){/*��ʾ���óɹ�������Ǩ������������ѭ��*/
				_hsthis->trans_num = ret;
				break;
			}
			else if (tryTimes==0){
				/*����ɾȥ���һ�飬�ٴγ��Է���*/
				tryTimes = 10;
				del_mig ++;
				//��ȡ�������һ�� ���û�ԭ�����ȵ��
				css.moveVMGtoPM(vmg_list[n_vmg-del_mig], 
					&_hsthis->sd->pmlist[
						_hsthis->sd->vmlist[
							vmg_list[n_vmg-del_mig]->vm_id_list[0]].at]);
			} else tryTimes--;

			if(del_mig==n_vmg)/*����������޷����ã�������ѭ��*/
				break;
		}

		//migrate_actionlist(vmg_list[0], css.old_atlist, n_vmg-del_mig);

		for(i=index=0;i<n_vmg-del_mig;i++){//�����߷���actionlist
			temp = vmg_list[i]->vm_id_list[0];
			pm1 = vmg_list[i]->at_pmid;//dst_pm��Ŀ�������
			pm2 = _hsthis->sd->vmlist[temp].at;//src_pm��Դ�����
			if(pm1 != pm2){//��Ŀ���������Դ�������ͬ��˵�����������壬д������б�
				for(j=0;j<vmg_list[i]->vm_total;j++){
					temp = vmg_list[i]->vm_id_list[j];
					css.SetActionlist(1, &_hsthis->sd->pmlist[pm2], 
						&_hsthis->sd->pmlist[pm1], &_hsthis->sd->vmlist[temp]);//���þ���
					index++;
				}
			}
		}
		
		if(del_mig>0){//�����޷����õ�������飬������ϢҲ��������б�
			action_unpackvm(del_mig, vmg_list[n_vmg-del_mig]);
			if(DEBUG)
				printf("Sorry, scheduling is failed, because pms' total available resource isn't enough.\n");
		}

		if(DEBUG){//��ӡ��������
			printf("There are %d actions.\n",index);
		}
	}
	else
		if(DEBUG)
			printf("Don't need migrate...\n");

	//�����ȵ���������ȵ����������������ʾ���
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
 * ��������selectVMG
 *  ���ܣ�ȡ��������Ϻ��ʵ���������������ȵ�
 * ���������pm��ʾ�������ȵ���������load��ʾ�������ȵ��ά��
 * ����ֵ�����������ȵ㣬�򷵻���������ַ���������������򷵻�NULL
 * create by sun 2013.8.4
 * modification : ������������ϵ������������Ǩ�Ƶ��ж�
 * modify by sun 2013.8.16
 */
#define fabs(x) ((x)>0?(x):(-x))
st_vm_group *selectVMG(pmPtr pm, int load)
{
	int i,n,j,stat,id;//��ʱ����
	int vmg_id;//�������ID
	double hot;//hotֵ
	double skw = LONG_MAX;//����ǡ�������ȵ��ƫ����
	int index = INVI;//�жϸ���������Ƿ�����������鶼����Ǩ�Ƶı��
	double temp;//��ʱ����

	hot = HOT;
	n= pm->countofvm;
	for(i=0;i<n;i++){
		vmg_id = pm->p_vms[i]->at_vmgid;
		temp = _hsthis->sd->vmglist[vmg_id].load[load].used 
			- (pm->host.load[load].used - pm->host.load[load].total * hot);//����ƫ������vmg.used>pm�ϳ����ȵ�ĸ��أ���temp>0

		for(j=0,stat=1;j<_hsthis->sd->vmglist[vmg_id].vm_total;j++){//�ų�stat=2��������飬����֤�������Ǩ��
			id = _hsthis->sd->vmglist[i].vm_id_list[j];
			if(_hsthis->sd->vmlist[id].host.stat==2)
				stat = 0;
		}
		if(fabs(temp)<skw&&stat==1){//�ҳ����Ըպ������ȵ��vmg
			index = vmg_id;
			skw = fabs(temp);
		}
	}

	if(index!=-1)/*��index��ֵ����������ϵ������������Ǩ�ƣ�����Null��ʾ����add by sun in 8-16*/
		return &_hsthis->sd->vmglist[index];
	else
		return NULL;
}

/*���޷����õ�������б�����actionlist*/
void action_unpackvm(int vmg_tot, vmgPtr vmg){
	actionPtr act;
	st_vm unpackvm_sum;//�޷����õ���������غ�
	vmPtr vm;
	int actionid, vmid, vm_tot;
	int i, j;//��ʱ����
	char msg[30];//��ʱ����

	actionid = _hsthis->sd->action_total++;//����������һ
	act = &_hsthis->sd->actionlist[actionid];
	act->action = 4;//action=4��ʾ�޷�����
	act->id = actionid;

	for(i=vm_tot=0;i<vmg_tot;i++){//���޷����õ������ָ�����δ���actionlist
		for(j=0;j<vmg[i].vm_total;j++){
			vmid = vmg[i].vm_id_list[j];
			act->unpackvm[vm_tot++] = &_hsthis->sd->vmlist[vmid];
		}
	}
	act->unpack_vm_total = vm_tot;//�����޷����õ����������

	for(i=0;i<vm_tot;i++){//������Щ������ĸ����ܺ�
		vm = act->unpackvm[i];
		if(!i){
			for(j=0;j<DIM;j++){
				strcpy(unpackvm_sum.host.load[j].metric_name, vm->host.load[j].metric_name);//������ά�ȵ����Ƴ�ʼ��
				unpackvm_sum.host.load[j].used  = vm->host.load[j].used;
			}
		}else{
			for(j=0;j<DIM;j++){
				unpackvm_sum.host.load[j].used += vm->host.load[j].used;
			}
		}
	}
	//����Ϣ����mesg
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
	act->msgtype = 2;//msgtype=2����ʾ��hotsolve�㷨�������޷�����������б�
}

//������̨�����
void swap_pm(pmPtr pm1, pmPtr pm2)
{
	st_pm temp;
	memcpy(&temp, pm1, sizeof(st_pm));
	memcpy(pm1, pm2, sizeof(st_pm));
	memcpy(pm2, &temp, sizeof(st_pm));
}

/* | | | | | | | | | | | | | | | | | | | | | | | | | | | | |���º������� | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | */

/*
 * ������ : transto_1D
 * ���� : ��ά��ʽ����
 * ������� : ��
 * ����ֵ : ��
 * @created by ��ҵ�� in 2013.5.27
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
 * ������ : sort_pm
 * ���� : �����������(����ѡ�����򣬴��Ľ�)
 * ������� : ��
 * ����ֵ : ��
 * @created by ��ҵ�� in 2013.5.27
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
 * ������ : if_allhot
 * ���� : ���������״̬�жϺ���
 * ������� : ��
 * ����ֵ : return 1��hot�� return 0��warm�� return -1��cold
 * @created by ��ҵ�� in 2013.5.27
 */
int if_allhot(void)
{
	st_pm hot__pm;
	st_vm hot__vm;
	int i,j,index;
	double warm;
	warm = (COLD+HOT)/2+(HOT-COLD)/4; /*����¶ȼ��㣬CJH Labled*/
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
 * ������ : if_onehot
 * ���� : ���������״̬�жϺ���
 * ������� : int pmID
 * ����pmID : ���жϵ������ID
 * ����ֵ : return 1��hot�� return 0��warm
 * @created by ��ҵ�� in 2013.5.27
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
 * ������ : hot_hotsolve
 * ���� : hot�����hotsolve����
 * ������� : int pmID, int load_num
 * ����pmID : �������ȵ�������
 * ����load_num : ����������ȵ�ά��
 * ����ֵ : ��
 * @created by ��ҵ�� in 2013.5.27
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
 * ������ : warm_hotsolve
 * ���� : warm�����hotsolve����
 * ������� : int pmID, int load_num
 * ����pmID : �������ȵ�������
 * ����load_num : ����������ȵ�ά��
 * ����ֵ : ��
 * @created by ��ҵ�� in 2013.5.27
 */
/*warm�����hotsolve����*/
/*
void warm_hotsolve(int pmID, int load_num)
{
	cold_hotsolve(pmID,load_num);
}
*/

/*
 * ������ : cold_hotsolve
 * ���� : cold�����hotsolve����
 * ������� : int pmID, int load_num
 * ����pmID : �������ȵ�������
 * ����load_num : ����������ȵ�ά��
 * ����ֵ : ��
 * @created by ��ҵ�� in 2013.5.27
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
					/ _hsthis->sd->pmlist[pmID].host.load[load_num].total) > used)		//�ҳ�ռ������ߵ�����������Ǩ��
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
 * ������ : hot_store
 * ���� : �����߽������v2p����
 * ������� : int vmid, int pm_src_id, int pm_dst_id
 * ����vmid : Ǩ�������ID
 * ����pm_src_id : Դ�����
 * ����pm_dst_id : Ŀ�������
 * ����ֵ : ��
 * @created by ��ҵ�� in 2013.5.27
 */
/*���������v2p����*/
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
 * ������ : output
 * ���� : ���߽������
 * ������� : ��
 * ����ֵ : ��
 * @created by ��ҵ�� in 2013.5.28
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
 * ������ : hot_update
 * ���� : ����Դ�������Ŀ����������غ���
 * ������� : int pm_src_id,int pmID,int vmID
 * ����pm_src_id : Դ�����ID
 * ����pmID : Ŀ�������ID
 * ����vmID : �����ID
 * ����ֵ : return 1�� ���³ɹ��� return 0������ʧ��
 * @created by ��ҵ�� in 2013.5.28
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
 * ������ : hot_pack
 * ���� : best fitװ��
 * ������� : int vmid
 * ����vmid : ���������������ʱ���ݽṹ�е�ID
 * ����ֵ : ��
 * @created by ��ҵ�� in 2013.5.28
 */
/*װ��*/
void hot_pack(int vmid)
{
	int i,j;
	int packed,vmID,pm_no = -1;
	double remained;//ʣ����
	double tmp,vol,max_vol = 0;
	vmID = _hsthis->vm_wait[vmid].id;
	while(pm_no == -1)
	{
		for(i=0;i<_hsthis->sd->pm_total;i++)//�������������
		{
			packed = 1;
			for(j=0;j<8;j++) 
			{
				remained = _hsthis->sd->pmlist[i].host.load[j].total*HOT - _hsthis->sd->pmlist[i].host.load[j].used;
				packed = (packed && (_hsthis->sd->vmlist[vmID].host.load[j].used < remained));//�ж��Ƿ��װ
			}
			if(packed)//�����װ������VOLֵ
			{
				tmp = 1;
				for(j=0;j<8;j++) tmp *= 1 - _hsthis->sd->vmlist[vmID].host.load[j].used/(_hsthis->sd->pmlist[i].host.load[j].reserved);
				vol = 1 / tmp;
				if(vol > max_vol)//�ҳ���װ��VOL���������
				{
					max_vol = vol;
					pm_no = i;
				}
			}
		}
		if(pm_no != -1) //�ҵ�ĳһ̨�������װ
		{
			_hsthis->sd->vmlist[vmID].at = pm_no;
			for(j=0;j<8;j++)
			{
				_hsthis->sd->pmlist[pm_no].host.load[j].used += _hsthis->sd->pmlist[vmID].host.load[j].used;//���¸���
				hot_store(vmID,_hsthis->vm_wait[vmid].pm_src_id,pm_no);
				break;
			}
		}
		else{		//�޿�װ������������������¿������
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

