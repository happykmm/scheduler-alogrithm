/*
  Module: loadbalance module
  Ҫ�������ȡ�������ݣ���ȡ�������ݣ���ȡPMʱ�Ӿ���
  ��������ؾ�������һ��Ǩ������
  Created by chenjianhai.
  Created date: 2013/05/20
*/

#include "alg_loadbalance.h"
#include "util.h"
/*
@�������ܣ����ؾ����㷨���ʼ��

����˵����THIS��ָ�����ָ��
*/

#define sqr(x) (x)*(x)

void init_loadbalance(cls_loadbalancePtr THIS){
	
	_lbthis=THIS;
	
	THIS->loadbalance=loadbalance;
}

/*���ò���*/
void setRequest_LB(cls_loadbalancePtr THIS)
{	
	/*��ȡ���ò���*/
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
	
		if (strcmp(p_req->type, "BALANCE")==0){

			for (iPara = 0 ; iPara< p_req->alg.paracount; iPara++){
				if (strcmp(p_req->alg.para[iPara].para_name,"balance")==0)
					THIS->sd->balance = p_req->alg.para[iPara].para_value;
				if (strcmp(p_req->alg.para[iPara].para_name,"maxcount")==0)
					THIS->sd->MaxcountOfBalance = p_req->alg.para[iPara].para_value;
				if (strcmp(p_req->alg.para[iPara].para_type,"DOUBLE_ARRAY")!=0)
					continue;
				split_str(p_req->alg.para[iPara].para_str_val,",",str_arr);
				for (i = 0;i<DIM;i++)
					if (strcmp(p_req->alg.para[iPara].para_name,"hot")==0)
						THIS->sd->hot_limit[i] = strtod(str_arr[i],NULL);
					else if (strcmp(p_req->alg.para[iPara].para_name,"cold")==0)
						THIS->sd->cold_limit[i] = strtod(str_arr[i],NULL);
					else if (strcmp(p_req->alg.para[iPara].para_name,"weight")==0)
						THIS->sd->weight[i] = strtod(str_arr[i],NULL);
			}
		}
	}
}
int cmp(int i,int j){
	return _lbthis->pmload[i]>_lbthis->pmload[j];
}

int vmgcmp(int i,int j){
	return _lbthis->vmgload[i] < _lbthis->vmgload[j];
}
/*
@�������ܣ����ؾ����㷨ʵ�ָ��ؾ������
����˵����
*/
void loadbalance()
{
	int pmname, vmname, pmnew;
	double bl, bl_o;
    double bl_before;
	int maxcount;
	int count;
	int flagvm, i, j, vmg_n;
	int order[PM_TOTAL];
	maxcount =_lbthis->sd->MaxcountOfBalance; // LB_COUNT; /*modified by cjh 08.21*/

	count = 0;
	flagvm = 0;	

	//�������ʼ��
	init_vmgroup(&cvg);
	cvg.sd = _lbthis->sd;
	_lbthis->sd->vmg_total=0;

	//sys���ʼ��
	initSys(&css);
	css.sd = _lbthis->sd;
	css.get_max_vec(dimMax);
	//��ÿ̨������ϵ����������
	i=0;
	while(i<_lbthis->sd->pm_total){
		cvg.grouping_in_pm(i);
		i++;
	}


	//��ÿ����������ڵĸ��ؼӺ�
	i=0;
	while(i<_lbthis->sd->vmg_total){
		css.SumVMG(i);
		//SumVMG(i);
		i++;
	}

	bl = css.getSysBalance();

    bl_before = bl;

	while (count < maxcount)
	{
		if (bl < _lbthis->sd->balance) break;
		calculatePMbyLoad();
		for (i = 0;i < _lbthis->sd->pm_total; i++)
			order[i] = i;
		sort(order, order+_lbthis->sd->pm_total, cmp);
		
		for (i= 0;i <_lbthis->sd->pm_total; i++){
			pmname = order[i];
			//if (_lbthis->pmload[pmname] < _lbthis->balance) break;

			vmg_n = getVMGinfoFromPM(pmname);
			sort(_lbthis->vmgorder, _lbthis->vmgorder+vmg_n, vmgcmp);

			for (j=0 ;j< vmg_n;j++){
				vmname = _lbthis->vmgorder[j];
				
				if (vmname == -1) 
				{
					fprintf(stderr,"After %d operations, cannot find a VM from PM %d.flagvm = %d\n"
						,count, pmname,flagvm);
					break;
				}

				pmnew = selectPMforVMG(pmname, vmname);
				if (pmnew != -1)
				{
					bl_o = bl;
					css.getVMGoutofPM(&_lbthis->sd->vmglist[vmname],&_lbthis->sd->pmlist[pmname]);
					//removeVMGfromPM(pmname, vmname);	
					css.moveVMGtoPM(&_lbthis->sd->vmglist[vmname],&_lbthis->sd->pmlist[pmnew]);
					//addVMGtoPM(pmnew, vmname);
					bl = css.getSysBalance();
					if (bl + EPSINON >= bl_o)
					{
						css.getVMGoutofPM(&_lbthis->sd->vmglist[vmname],&_lbthis->sd->pmlist[pmnew]);
						css.moveVMGtoPM(&_lbthis->sd->vmglist[vmname],&_lbthis->sd->pmlist[pmname]);
						bl =  css.getSysBalance();
					}else {
						goto oneLoop;
					}
				}
			}
		}

oneLoop:


		count++;
		bl =  css.getSysBalance();
		if (bl < _lbthis->sd->balance) break;
	}

	_lb_output();

    printf("After %d Load balance degree: %f %f\n", count, bl_before, bl);

	return;
}



void _lb_output()
{
	int i, temp, pm1, pm2, j;
	for(i=0;i<_lbthis->sd->vmg_total;i++){//�����߷���actionlist
			temp = _lbthis->sd->vmglist[i].vm_id_list[0];
			pm1 = _lbthis->sd->vmglist[i].at_pmid;//dst_pm��Ŀ�������
			pm2 = _lbthis->sd->vmlist[temp].at;//src_pm��Դ�����
			if(pm1 != pm2){//��Ŀ���������Դ�������ͬ��˵�����������壬д������б�
				for(j=0;j<_lbthis->sd->vmglist[i].vm_total;j++){
					temp = _lbthis->sd->vmglist[i].vm_id_list[j];
					css.SetActionlist(1, &_lbthis->sd->pmlist[pm2], 
						&_lbthis->sd->pmlist[pm1], &_lbthis->sd->vmlist[temp]);//���þ���

				}
			}
		}

}


/*���㵱ǰ������ı���6�����ȣ�1ΪǨ�룬0ΪǨ��*/
double calculatePMAfter(int pmname, int vmgname, int flag)	
{
	double bl;
	
	double load_n;
	//double avg;
	int pmnow, vmnow,j,i;
	double maxload [DIM];

	
	css.get_max_vec(maxload);
	for (i= 0 ; i< DIM ;i++)
		_lbthis->avg[i] = css.getSysAvgUsed(i);
	load_n = 0;

	if (flag == 0)
	{
		
		//�Ƴ���ǰvm���pm ÿһά�ȵ�ʹ���ʣ�����
		for (j=0;j<DIM;j++)
			if (_lbthis->sd->pmlist[pmname].host.load[j].total>0)
				load_n = load_n + sqr((_lbthis->sd->pmlist[pmname].host.load[j].used 
						- _lbthis->sd->vmglist[vmgname].load[j].used
						+_lbthis->sd->pmlist[pmname].host.load[j].reserved)/_lbthis->sd->pmlist[pmname].host.load[j].total
						-_lbthis->avg[j])*_lbthis->sd->weight[j];

		bl = load_n;

	}
	else
	{
		//���뵱ǰvm���pm ÿһά�ȵ�ʹ���ʣ�����
		for (j=0;j<DIM;j++)
			if (_lbthis->sd->pmlist[pmname].host.load[j].total>0)
				load_n = load_n + sqr((_lbthis->sd->pmlist[pmname].host.load[j].used 
						+ _lbthis->sd->vmglist[vmgname].load[j].used
						+_lbthis->sd->pmlist[pmname].host.load[j].reserved)/_lbthis->sd->pmlist[pmname].host.load[j].total
						-_lbthis->avg[j])*_lbthis->sd->weight[j];
		bl = load_n;

	}

	return bl;
}



/*Ϊѡ��õ�vmѡ��һ̨pmǨ�룬���ܷŻ�ԭpm*/
int selectPMforVMG(int pmname_o, int vmgname)
{
	double min;
	double blnow;
	int flag;
	int pmname;
	int pm_temp;
	int vm_temp;
	int i;
	cls_st_sys css;
	initSys(&css);
	css.sd = _lbthis->sd;

	
	min = 10000;
	flag = 1;
	for (i = 0;i < _lbthis->sd->pm_total; i++)
		if (i!=pmname_o)
		{
			//if (pm_temp->canFit(vm_temp))//(checkVMtoPM(vmname, (*(*piter)).id) == 0)
			if (_lbthis->sd->pmlist[i].countofvm>0
				&& css.can_be_packed(vmgname,i,_lbthis->sd->vmglist)
				&& css.checkPlaceAff(&_lbthis->sd->vmglist[vmgname],&_lbthis->sd->pmlist[i]))
			{
				blnow = calculatePMAfter(i, vmgname, 1);
				if (blnow < min)
				{
					pmname = i;
					min = blnow;
					flag = 0;
				}
			}
		}

	

	if (flag == 0)	return pmname;
	else return -1;
}

/*��ָ���������ѡ�������Ǩ�Ƶ�vm��*/
int getVMGinfoFromPM(int pmname)
{
	double total;
	double min1, min2;
	//double loadnow;
	int vmname1, vmname2;
	int i,j,id;
	int ret_n = 0;
	double maxload [DIM];

	css.get_max_vec(maxload);
	min1 = 10000;
	min2 = min1 + 1;
	vmname1 = vmname2 = -1;
	for (i= 0 ; i< DIM ;i++)
		_lbthis->avg[i] = css.getSysAvgUsed(i);
	for (i = 0 ;i <_lbthis->sd->vmg_total; i++)
	if (_lbthis->sd->vmglist[i].at_pmid==pmname){
		total = 0;
		for (j=0; j< _lbthis->sd->vmglist[i].vm_total; j++)
		{
			id = _lbthis->sd->vmglist[i].vm_id_list[j];
			if(_lbthis->sd->vmlist[id].host.stat==2)
						break;
		}
		if (j< _lbthis->sd->vmglist[i].vm_total)
			continue;
		for (j=0;j <DIM;j++)
				total = total + sqr((_lbthis->sd->pmlist[pmname].host.load[j].used 
							- _lbthis->sd->vmglist[i].load[j].used
							+_lbthis->sd->pmlist[pmname].host.load[j].reserved)
								/_lbthis->sd->pmlist[pmname].host.load[j].total
							- _lbthis->avg[j])*_lbthis->sd->weight[j];
		_lbthis->vmgload[i] = total;
		_lbthis->vmgorder[ret_n++] = i;

	}
	return ret_n;
}


/*0ѡ������ߵ�pmid���أ�1ѡ�������*/
int sortPMbyLoad(int flag)
{		
	double max, min;
	int minpm, maxpm, i;

	max = 0;
	min = 10000;
	
		
	for (i=0;i < _lbthis->sd->pm_total; i++)
	{
		if (max < _lbthis->pmload[i]) 
		{
			max = _lbthis->pmload[i];
			maxpm = i;
		}
		if (min > _lbthis->pmload[i]) 
		{
			min = _lbthis->pmload[i];			
			minpm = i;
		}
	}

	if (flag == 0) return maxpm;
	else return minpm;
}

/*����pm���أ�����pmload��map*/
int calculatePMbyLoad()
{

	double total;		
	int i,j;
	//����ÿ��PM��ʹ���ʼ�����ľ����
	for (i=0;i < _lbthis->sd->pm_total; i++)
	{
		_lbthis->pmload[i] = css.getPMSkw(&_lbthis->sd->pmlist[i]);
	}

	return _lbthis->sd->pm_total;
}

double getSquareVar(st_pm pm, double avg, int name){
	return sqr((pm.host.load[name].used-avg)/pm.host.load[name].total);
}