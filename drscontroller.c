#include "drscontroller.h"


/*Initial datcontrol struct object*/
void init_drscontrol(cls_st_drscontrol* THIS)
{	
	_dcthis=THIS;
	
	//THIS->execute=_execute
	THIS->parseXML1=parseXML1;
	THIS->parseXML2=parseXML2;
	THIS->parseXML21=parseXML21;
	THIS->parseXML22=parseXML22;

	THIS->constructXML2=constructXML2;
	THIS->constructXML21=constructXML21;
	THIS->constructXML22=constructXML22;
	THIS->constructXML3=constructXML3;

	//THIS->generate_all_data=generate_all_data;
	THIS->execute_sched=execute_sched;

	THIS->show_pmlist=show_pmlist;
	THIS->show_vmlist=show_vmlist;

	THIS->setDefaultRequest=setDefaultRequest;
	
}



/*///////////////////////////////////////////////////////////////////*/
/*二、主要函数声明*/

/***1.XML解析有关的类*************************************/

/* Read xml from src (from either file or memory) */
int readXML(int type, char *src, char *dst, char *dir)
{
	return 1;		
}

/* Parse xml1 to requestList */
void parseXML1(char *filename)
{
	cls_st_xmlprocess xp;
	init_xmlprocess(&xp);
	xp.sd=_dcthis->sd;
	xp.get_request_from_xml1(filename);
	
}

/* Parse xml2 to VMList and PMList */
void parseXML2(char *filename){
	cls_st_xmlprocess xp;
	init_xmlprocess(&xp);
	xp.sd=_dcthis->sd;
	xp.get_vmpm_from_xml2(filename);
}

/* Parse xml21 to p2pList 时延-距离-矩阵*/
void parseXML21(char *filename){
	cls_st_xmlprocess xp;
	init_xmlprocess(&xp);
	xp.sd=_dcthis->sd;
	xp.get_pm2pm_from_xml21(filename);
		
		//get_pm2pm_from_xml21(filename);	
}

/* Parse xml22 to v2vList VM流量-矩阵*/
void parseXML22(char *filename)
{
	cls_st_xmlprocess xp;
	init_xmlprocess(&xp);
	xp.sd=_dcthis->sd;
	xp.get_vm2vm_from_xml22(filename);

	return;
}

/* Parse xml3 to actionList 调度决策实施操作列表*/
void parseXML3(char *filename)
{
	
	return;
}

/*////////////////////////////////////////////////////////////////////////////*/

/* Construct xml2 based on vmid2vm and pmid2pm  = "data_output.xml"*/
void constructXML2(char *filename)
{	
	cls_st_xmlprocess xp;
	init_xmlprocess(&xp);
	xp.sd=_dcthis->sd;
	xp.xml_create_by_pms_vms(filename);	
}

/* Construct xml21 based on p2pList 时延-距离-矩阵*/
void constructXML21(char *filename)
{
	cls_st_xmlprocess xp;
	init_xmlprocess(&xp);
	xp.sd=_dcthis->sd;
	xp.xml_create_by_pm2pm_treetopo(filename);	
	return;
}

/* Construct xml22 based on v2vList VM流量-矩阵*/
void constructXML22(char *filename)
{
	cls_st_xmlprocess xp;
	init_xmlprocess(&xp);
	xp.sd=_dcthis->sd;
	xp.xml_create_by_vm2vm(filename);	
	return;
}

/* Construct xml3 based on the actionList */
void constructXML3(char *filename)
{
	cls_st_xmlprocess xp;
	init_xmlprocess(&xp);
	xp.sd=_dcthis->sd;
	xp.xml_create_by_actions(filename);	

	return;
}


/*****************************************************************************/

/**||||||2.生成模拟数据有关的函数||||||||||||||||||||||*/

/* Generate PM list data */
void generate_pmlist(int pm_num, pmPtr lstpm)
{
	return;
}
/* Generate VM list data */
void generate_vmlist(int vm_num, vmPtr lstvm)
{
	return;
}	
/* Generate v2v list data -虚拟机流量数据*/
void generate_v2vlist(int vm_num, float vr){
	return;
}
/* Generate p2p list data -物理机时延距离数据*/
void generate_p2plist(int pm_num){
	return;
}	



/*****************************************************************************/

/**||||||3.调度系统有关的函数||||||||||||||||||||||*/

/* Set default request
modified by cjh 2013-08-21
*/
void setDefaultRequest(){
	int i;
	requestPtr p_req;
	for(i=0;i<_dcthis->sd->req_total;i++)
	{
		p_req=&_dcthis->sd->reqlist[i];
		if(strcmp(p_req->type,"BALANCE")==0) 
			_dcthis->sd->req_lb=p_req;
		else if(strcmp(p_req->type,"PLACE")==0)
			_dcthis->sd->req_pl=p_req;
		else if(strcmp(p_req->type,"CONSOLIDATE")==0)
			_dcthis->sd->req_cs=p_req;
		else if(strcmp(p_req->type,"HOTSPOTSOLVE")==0)
			_dcthis->sd->req_hs=p_req;
	}

}


void insertIffEdge(int vm, vm2vmPtr Edge,int needSwap){
	int nxt = _dcthis->sd->afflist_total;
	vm2vmPtr newEdge = &_dcthis->sd->afflist[_dcthis->sd->afflist_total++];
	memcpy(newEdge, Edge, sizeof(st_vm2vm));
	if (needSwap)
	{
		vmPtr tmp = newEdge->p_src_vm ;
		newEdge->p_src_vm = newEdge->p_dst_vm;
		newEdge->p_dst_vm = tmp;
	}
	_dcthis->sd->afflist[nxt].nextPtr = _dcthis->sd->vmlist[vm].head;
	_dcthis->sd->vmlist[vm].head = nxt;
}
	
void init_global_data(){
	int i,j, id;

	//亲和关系的正向表
	for (i=0; i<_dcthis->sd->vm_total; i++)
		_dcthis->sd->vmlist[i].head = -1;

	for (i = 0;i <_dcthis->sd->v2v_total ;i++)
	{
		vm2vmPtr Edge = &_dcthis->sd->v2vlist[i];

		if (Edge->affinity <= 0){
			//二元亲和性
			insertIffEdge(_dcthis->sd->v2vlist[i].p_src_vm->host.id, Edge, 0);

			insertIffEdge(_dcthis->sd->v2vlist[i].p_dst_vm->host.id, Edge, 1);
		}else if (Edge->affinity >0){
			//多元亲和性 1, 2, 3
			for (j=0;j < Edge->countofvm; j++)
			{
				id = Edge->p_vms[j]->host.id;
				insertIffEdge(id, Edge, 0);
			}
			//if(DEBUG)printf("%d",j);
		}
	}

	/*
	遍历下标src的虚拟机的所有二元亲和性
	int src;
	for (i = _dcthis->sd->vmlist[src].head ; i!=-1 ; i = _dcthis->sd->afflist[i].nextPtr){
		vm2vmPtr newrelation = &_dcthis->sd->afflist[i];
	}
	*/


	
}



/*****************************************************************************/
/*获取（判断）系统状态；*/
int get_sysstat(){




	return 1;
}		 



/**||||||其他相关函数||||||||||||||||||||||*/
/*干什么活*/
void do_action(int action)
{
	int schaction;
	switch(action){
	case DOGENERATEDATA:/*数据生成*/
		//generate_all_data();
		break;
	case DOSCHEDULE: /*调度*/
		/*enum ScheduleActions {DOINITPLACE = 1, DOLOADBALANCE = 2, DOCONSOLIDATE = 3, DOHOTSOLVE = 4 };*/
		printf("Input schedule action:1-DOINITPLACE, 2-DOLOADBALANCE,3-DOCONSOLIDATE, 4-DOHOTSOLVE.\n");
		scanf("%d",&schaction);		
		//exec_shed(schaction);
		break;
	case DOPREDICT:		
		break;
	}	
}


//输出所有的物理机信息
void show_pmlist()
{
	int pt;	int i,j;
	pmPtr pmlist;

	cls_st_host sh;
	
	initHost(&sh);
	sh.sd=_dcthis->sd;

	pmlist=_dcthis->sd->pmlist;

	pt=_dcthis->sd->pm_total; 

	printf("pmid     hostip    stat   \n");
		
	for(i=0;i<pt;i++){
		//PM property:
		printf("pm:\n %6s  %10s  %3.f \nmetrics: used    total    reserved\n",
			pmlist[i].host.sid,pmlist[i].host.ip,pmlist[i].host.stat);
		
		for(j=0;j<DIM;j++){
			printf("%6s",sh.getName(j));
			printf("%10.3f",pmlist[i].host.load[j].used);
			printf("%10.3f",pmlist[i].host.load[j].total);
			printf("%10.3f\n",pmlist[i].host.load[j].reserved);
		}

		printf("\n");
	}	
}


//输出所有的VM信息
void show_vmlist()
{
	int pt;
	int i,j;
	vmPtr vmlist;
	cls_st_host sh;
	
	initHost(&sh);
	sh.sd=_dcthis->sd;

	vmlist=_dcthis->sd->vmlist;
	
	pt= _dcthis->sd->vm_total; 
	printf("vmid     hostip    stat    atpm\n");
		
	for(i=0;i<pt;i++){
		//PM property:
		printf("vm:\n %6s  %10s  %3.f ",
			vmlist[i].host.sid,vmlist[i].host.ip,vmlist[i].host.stat);

		printf("%s\n", _dcthis->sd->pmlist[vmlist[i].at].host.sid);

		printf("metrics: used    total    reserved\n");
		for(j=0;j<DIM;j++){
			printf("%6s",sh.getName(j));
			printf("%10.3f",vmlist[i].host.load[j].used);
			printf("%10.3f",vmlist[i].host.load[j].total);
			printf("%10.3f\n",vmlist[i].host.load[j].reserved);
		}
		printf("\n");
	}	
}



/*
函数名称：void execute_sched(int decistat)
函数功能：执行调度算法
参数： int decistat; 
--调度决策类型 DOINITPLACE，DOLOADBALANCE，DOCONSOLIDATE,DOHOTSOLVE
创建日期：2013-5-05
*/
void execute_sched(int decistat)
{
	cls_st_scheduler p_sched;
	cls_st_pm cls_pm;
	cls_st_net cls_net;
	init_scheduler(&p_sched);
	p_sched.sd=_dcthis->sd;
	
	initNet(&cls_net);
	cls_net.sd = _dcthis->sd;
	initPm(&cls_pm);
	cls_pm.sd = _dcthis->sd;
	cls_pm.set_fr_gr_id();
	cls_net.update_net();
	cls_net.getdelay();
	cls_net.get_pmport_at_switch();
	cls_net.preparePMnearest();
	//init_v2vlist();
	init_global_data();
	switch(decistat)
	{
	case DOINITPLACE:/*初试放置*/
		p_sched.initplace();

		break;
	
	case DOLOADBALANCE:/*负载均衡*/
		p_sched.loadbalance();
		break;
	
	case DOCONSOLIDATE:/*整合*/
		p_sched.consolidation();
		break;	
	case DOHOTSOLVE:/*热点消除*/
		p_sched.hotsolve();
		break;
	default:
		break;
	}

}


void init_v2vlist()
{
	int i,j;
	int v2v_tot, vm_tot, vm_id;
	vm2vmPtr v2v;

	v2v_tot = _dcthis->sd->v2v_total;

	for(i=0;i<v2v_tot;i++){

		vm_tot = _dcthis->sd->v2vlist[i].countofvm;
		v2v = &_dcthis->sd->v2vlist[i];
		j=0;

		if(vm_tot == 2){
			vm_id = get_vmid_in_vmids(&v2v->vmids[j],&j);
			v2v->p_src_vm = &_dcthis->sd->vmlist[vm_id];
			vm_id = get_vmid_in_vmids(&v2v->vmids[j],&j);
			v2v->p_dst_vm = &_dcthis->sd->vmlist[vm_id];
		}else{
			while(j<vm_tot){
				vm_id = get_vmid_in_vmids(&v2v->vmids[j],&j);
				v2v->p_vms[j] = &_dcthis->sd->vmlist[vm_id];
				j++;
			}
		}
	}
}

int get_vmid_in_vmids(char * id, int * j){
	int n=0;
	(*j)++;
	while(*id != ',' && *id != '\0'){
		if(*id > '0' && *id< '9'){
			n *= 10;
			n += *id - '0';
		}
		id++;(*j)++;
	}
	return n;
}