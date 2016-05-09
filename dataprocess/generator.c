
/* 文件注释 */
/*
* @file generate.c generate the simulation data
* @created by  sunyecheng <dangoakachan@foxmail.com> at 2013.05.11
* @modified by Jianhai Chen at 2013.05.13
*/

#include "generator.h"


/*容量规格，4种PM*/
st_PMResVector PV={
	{400,800,1600,3200},/*100%百分比*/
	{8192,16384,32768,65536}, /*MB*/
	{512,1024,2048,4096},/*GB*/
	{100,100,100,100},/*100%*/
	{1024,2048,3072,4096},/*mbps*/
};

st_pm PMV[4];
st_pm minPMV;


/*模拟数据：生成PMV规格4种*/
void createPMV()
{
	int i,j;
	
	/*模板VM，VM的负载=模板VM*一个随机数如0.3*/
	minPMV.host.load[CPU].total=400;
	minPMV.host.load[MEM].total=4096;
	minPMV.host.load[DISKIO].total=8;/*zGB*/
	minPMV.host.load[FPGA].total=50;
	minPMV.host.load[ETH0].total=512;
	minPMV.host.load[ETH1].total=512;
	minPMV.host.load[ETH2].total=512;
	minPMV.host.load[ETH3].total=512;

	for(i=0;i<4;i++)
	{
		PMV[i].host.load[CPU].total=PV.pcpu[i];
		PMV[i].host.load[CPU].used=0;
		PMV[i].host.load[CPU].reserved=0;
		PMV[i].host.load[MEM].total=PV.pmem[i];
		PMV[i].host.load[MEM].used =0;
		PMV[i].host.load[MEM].reserved =0;

		PMV[i].host.load[DISKIO].total=PV.pdisk[i];
		PMV[i].host.load[DISKIO].used=0;
		PMV[i].host.load[DISKIO].reserved=0;

		PMV[i].host.load[FPGA].total=PV.pfpga[i];
		PMV[i].host.load[FPGA].used=PMV[i].host.load[FPGA].reserved=0;

		j=(int)(4*random_021());
		PMV[i].host.load[ETH0].total=PV.pnet[j];
		PMV[i].host.load[ETH0].used=PMV[i].host.load[ETH0].reserved=0;

		j=(int)(4*random_021());
		PMV[i].host.load[ETH1].total=PV.pnet[j];
		PMV[i].host.load[ETH1].used=PMV[i].host.load[ETH1].reserved=0;

		j=(int)(4*random_021());
		PMV[i].host.load[ETH2].total=PV.pnet[j];		
		PMV[i].host.load[ETH2].used=PMV[i].host.load[ETH2].reserved=0;

		j=(int)(4*random_021());
		PMV[i].host.load[ETH3].total=PV.pnet[j];
		PMV[i].host.load[ETH3].used=PMV[i].host.load[ETH3].reserved=0;
	}
}


/***********************************************************************/
/*
* 函数名 : void init_dataprocess(cls_dataprocessPtr THIS)
* 功能 : 初始化数据处理类的成员变量
* 传入参数 : cls_dataprocessPtr THIS 数据处理类指针
* 返回值 :无
* @created by Jianhai Chen in 2013.5.12
*/

void init_generator(cls_generatorPtr THIS)
{
	_gthis=THIS;   /*初始化内部this指针*/


	THIS->set_seed=set_seed;			//获取随机数种子
	THIS->create_frame=create_frame;
	THIS->create_group=create_group;

	THIS->random_021=random_021;		//随机生成区间0-1内的实数
	THIS->random_a2b=random_a2b ;	//随机生成区间a-b内的实数
	THIS->random_int_ab=random_int_ab;//随机生成整数a或b
	THIS->ifiprepeat=ifiprepeat;		//检测随机生成的ip地址是否重复
	THIS->ifv2vrepeat=ifv2vrepeat;		//检测随机生成的vm2vm是否重复
	THIS->updatepm=updatepm;			//产生一个虚拟机后,更新物理机
	THIS->create_pm=create_pm;			//随机生成物理机
	THIS->create_vm=create_vm;			//随机生成虚拟机
	THIS->create_v2v=create_v2v;		//随机生成vm流量关系
	THIS->create_v2v_no_aff=create_v2v_no_aff;		//随机生成vm非流量亲和关系
	THIS->generate_data_by_vms=generate_data_by_vms;//随机生成vm非流量亲和关系
	THIS->create_v2v_by_aff_type=create_v2v_by_aff_type;	//随机生成亲和关系
	THIS->create_v2v_by_ta=create_v2v_by_ta;
	THIS->clear_res=clear_res;			//将reserved全置为0
	THIS->count=count;					//统计热点机数和冷点机数
	THIS->set_stat=set_stat;			//将非空物理机的stat置为1,空物理机置为0
	THIS->createPMV=createPMV;
	THIS->addtov2vlist=addtov2vlist;
	THIS->create_empty_pm=create_empty_pm;
	THIS->set_empty_pm=set_empty_pm;	
	
	THIS->create_p2p_tree_topo=create_p2p_tree_topo;	
}



//
///***********************************************************************/
///*
// * 函数名 : void generator_fun(void)
// * 功能 : 模拟数据生成函数,用于集成调用虚拟机生成和物理机生成函数
// * 传入参数 : 无
// * 参数1 : 
// * 返回值 : 无
// * @created by 孙业成 in 2013.5.11
// */
//void generator_fun(void)
//{
//	double lower,upper;									//定义a，b用于存储lower bound和upper bound
//	double total;
//	printf("please input total number of pm\n");
//	scanf("%d",&g_total_pm);								//输入物理机总数
//	printf("please input total number of vm\n");
//	scanf("%d",&g_total_vm);								//输入虚拟机总数
//	printf("please input α and β :(α is lower bound, β is upper bound)\n");
//	printf("α（0,1） = ");
//	scanf("%lf",&lower);										//输入负载下限
//	printf("β（0,1） = ");
//	scanf("%lf",&upper);										//输入负载上限
//
//	create_pm();
//	create_vm(lower,upper);
//
//	printf("please input traffic_rate lower-bound and upper-bound:\n");
//	printf("lower bound(0,10000) = ");
//	scanf("%lf",&lower);										//输入流量下限
//	printf("upper bound(0,10000) = ");
//	scanf("%lf",&upper);										//输入流量上限
//	printf("total border(0.2 or 0.3 or 0.4) = ");
//	scanf("%lf",&total);										//输入边数
//	create_v2v(lower,upper,total);
//}


/***********************************************************************/
/*
* 函数名 : void set_seed(void)
* 功能 : 获取随机数种子
* 传入参数 : 无
* 参数1 : 
* 返回值 : 无
* @created by 孙业成 in 2013.5.11
*/
void set_seed(void)	
{
	srand((unsigned) time(NULL));//获取系统时间作为随机数种子
}


/***********************************************************************/
/*
* 函数名 : double random(void)
* 功能 : 随机产生区间[0,1)内的实数
* 传入参数 : 无
* 参数1 : 
* 返回值 : 随机产生的实数
* @created by 孙业成 in 2013.5.11
*/
double random_021(void)
{
	return rand()/(RAND_MAX+1.0);
}


/***********************************************************************/

/*
* 函数名 : double random_a2b(double a,double b)
* 功能 : 随机产生区间[a,b]内的实数
* 传入参数 : double a, double b
* 参数a : 区间下限
* 参数b : 区间上限
* 返回值 : 随机产生的实数
* @created by 孙业成 in 2013.5.11
*/
double random_a2b(double a,double b)//随机产生区间[a,b]内的实数
{
	return (b-a)*rand()/(RAND_MAX+0.0)+a;
}

/***********************************************************************/
/*
* 函数名 : int random_int_ab(int a,int b)
* 功能 : 随机返回整数a或b
* 传入参数 : int a, int b
* 参数a : 传入的变量
* 参数b : 传入的变量
* 返回值 : 随机产生的a或b
* @created by 孙业成 in 2013.5.11
*/
int random_int_ab(int a,int b)
{
	if(!(rand()%2))					//rand()为偶数返回a,否则返回b
		return a;
	else
		return b;
}

/***********************************************************************/
/*
* 函数名 : int ifiprepeat(int i,int j,int index)
* 功能 : 判断ip地址是否重复，从0--i-1台中
* 传入参数 : int i,int j,int index
* 参数i : 第i台pm或vm
* 参数j : pm或vm的第j个ip(共四个)
* 参数index :index=0表示pm,1表示vm
* 返回值 : return 1表示ip地址无重复, return 0表示有重复
* @created by 孙业成 in 2013.5.11
*/
int ifiprepeat(int i,int j,int index)
{
	int k,l;
	if(!index){	/*根据输入的参数判断是检测vm还是pm*/
		for(k=0;k<=i;k++)
			for(l=0;l<PORT_NUM;l++)
				/*将传入的pm的ip与所有pm的ip地址对比*/
					if((!strcmp(_gthis->sd->pmlist[i].host.eip[j],
						_gthis->sd->pmlist[k].host.eip[l]))&&(k!=i&&l!=j))	
						return 0; //相等
	}
	else{
		for(k=0;k<i;k++)
			if(!strcmp(_gthis->sd->vmlist[i].host.ip,_gthis->sd->vmlist[k].host.ip))					//将传入的vm的ip与所有vm的ip地址对比
				return 0;//重复
	}



	return 1;//无重复
}


/***********************************************************************/
/*
* 函数名 : int updatepm(int vmID,int pmID)
* 功能 : 每产生一台vm后更新其宿主pm的状态
* 传入参数 : int vmID,int pmID
* 参数vmID : 虚拟机的ID
* 参数pmID : 物理机的ID
* 返回值 : return 1表示更新成功, return 0表示超出物理机容量
* @created by 孙业成 in 2013.5.11
*/
int updatepm(int vmID,int pmID)
{
	double temp,resd,used;//临时变量,用于存储计算的中间值
	int k;

	/*预判断是否放得下PM*/
	for(k=0;k<DIM;k++){	//依次对8个维度进行更新
		resd=_gthis->sd->pmlist[pmID].host.load[k].total - _gthis->sd->pmlist[pmID].host.load[k].reserved;
		used=_gthis->sd->vmlist[vmID].host.load[k].used + _gthis->sd->pmlist[pmID].host.load[k].used;
		temp =  resd- used;	//temp等于更新后的剩余容量
		if(temp<0)	//如果temp小于0,表示物理机容量不够,返回0
			return 0;
	}

	/*完成放置，并更新PM剩余容量*/
	for(k=0;k<DIM;k++)
		_gthis->sd->pmlist[pmID].host.load[k].used 
		+= _gthis->sd->vmlist[vmID].host.load[k].used;//更新该物理机的used

	return 1;
}


/***********************************************************************/
/*
* 函数名 : int createframe(int)
* 功能 : 随机创建框列表，基于PM
* 传入参数 : num每个框的PM个数
* 参数1 : 
* 返回值 : 成功-1，失败-0， _gthis->framelist中保存着信息
* @created by 陈建海 in 2013.7.22
* @modify by ## in ####.##.##
*/
int create_frame(int num)
{
	int i,j;

	framePtr p_fr;
	pmPtr p_pm;

	/*总共所需要的框的个数100/18 = 5 10 */
	//fcc=_gthis->sd->pm_total/num+1;

	for(j=0;j<_gthis->sd->pm_total;j++)	
	{
		/*获取一个PM*/
		p_pm=&_gthis->sd->pmlist[j];

		if(j%num==0) {
			i=j/num;
			p_fr=&_gthis->sd->framelist[i];
			p_fr->countofpm=0;
			strcpy(p_fr->pmids,p_pm->host.sid);/*加入PMSID到框*/
			p_fr->id=i;
			sprintf(p_fr->sid,"fr%d",i);
			_gthis->sd->fr_total++;
		}
		else{/*加入新的PM*/
			sprintf(p_fr->pmids,"%s,%s",p_fr->pmids,p_pm->host.sid);
		}
		
		p_fr->i_pmids[j%num]=p_pm->host.id; /*加入pm.id到该框*/
			
		/*计数加1*/
		p_fr->countofpm++;
		
		/*更新PM所在框*/
		p_pm->at_p_fr=p_fr;/*更新所在框指针*/		
		strcpy(p_pm->at_fr_sid,p_fr->sid);
	}

	return  _gthis->sd->fr_total;
}



/***********************************************************************/
/*
* 函数名 : int create_group(int)
* 功能 : 随机抽取若干PM，生成一个组基于PMLIST
* 传入参数 : num, pm的个数
* 参数1 : 
* 返回值 : 成功-1，失败-0， _gthis->grouplist中保存着信息
* @created by 陈建海 in 2013.7.22
* @modify by ## in ####.##.##
*/
int create_group(int start,int num)
{
	int i,j,kk,pid,flag,pmtot,grtot;
	pmgPtr p_gr;
	pmPtr p_pm;
	//_gthis->sd->gr_total=num;

	pmtot=_gthis->sd->pm_total;
	grtot=_gthis->sd->gr_total;/*在gr_total的基础上继续生成新的组*/

	/*取一个组*/	
	p_gr=&_gthis->sd->pmglist[grtot];		
	p_gr->id=grtot;
	sprintf(p_gr->sid,"gr%d",grtot);
	p_gr->countofpm=0;

	/*获取num个PM*/
	for(i=0;i<num;i++){
		/*生成没有重复的PM*/
		flag=1;
		while(flag==1)
		{
			kk=start+i;//(int)(random_021()*pmtot); 
			p_pm=&_gthis->sd->pmlist[kk];
		
			/*检查是否已经存在*/
			flag=0;
			for(j=0;j<i;j++)
			{
				pid=p_gr->i_pmids[j];
				if(kk==pid) {
					flag=1;break;
				}			
			}
		}

		/*找到了p_pm*/
		p_gr->i_pmids[i]=p_pm->host.id;
		p_gr->countofpm++;

		if(i==0)
			sprintf(p_gr->pmids,"%s",p_pm->host.sid);
		else
			sprintf(p_gr->pmids,"%s,%s",p_gr->pmids,p_pm->host.sid);		
		
		/*更新PM所在组信息*/
		p_pm->at_p_pmgrps[p_pm->countofpmgrp]=p_gr;
		
		if(p_pm->countofpmgrp==0)
			strcpy(p_pm->at_gr_ids,p_gr->sid);
		else
			sprintf(p_pm->at_gr_ids,"%s,%s",p_pm->at_gr_ids,p_gr->sid);
		p_pm->countofpmgrp++;

	}

	/*组创建完毕，数量增加1*/
	_gthis->sd->gr_total++;

	return 1;
}

/***********************************************************************/
/*
* 函数名 : void createIP(char *ip12, char *retip)
* 功能 : 获得一个IP地址
* 传入参数 : (char *ip12, char *retip)
* 参数ip12 : IP地址的前两个段数码字符串：如(10.10....)
* 参数retip: 指向生成后返回的IP: 包含4个段（10.10.2.234）
* 返回值 : 无
* @created by 陈建海 in 2013.7.13
*/

void createIP(char *ip12, char *retip)
{
	int t1,t2;
	char rip[20];
	t1=(int)random_a2b(1,255);//生成第3段数码
	t2=(int)random_a2b(1,255); //生成第4段数码
	sprintf(retip,"%s.%d.%d",ip12,t1,t2);
}




/***********************************************************************/
/*
* 函数名 : int createpm(int)
* 功能 : 随机创建物理机列表
* 传入参数 : pm_num, 物理机的台数
* 参数1 : 
* 返回值 : 成功-1，失败-0， _gthis->pmlist中保存着信息
* @created by 孙业成 in 2013.5.11
* @modify by sun in 2013.6.17
*/
int create_pm(int num,int hotpm,int coldpm)
{
	int i,j,d;	//i,j均为循环式使用变量
	int temp1,temp2;	//temp1,temp2用于存放ip地址的后两段
	double reserved; //用于存放随机生成reserved的随机数
	char eth[10];/*网络层面的名称*/

	set_seed();		//获取随机数种子
	for(i=0;i<num;i++){//每创建一台pm循环一次

		_gthis->sd->pmlist[i].host.id = i;//id按顺序排列*/
		sprintf(_gthis->sd->pmlist[i].host.sid,"pm%d",i);
		
		for(j=0;j<PORT_NUM;j++){
			/*生成IP地址*/
			temp1=(int)random_a2b(1,255);//temp等于1到255间的整数(包括1,但不包括255)
			temp2=(int)random_a2b(1,255);
			sprintf(_gthis->sd->pmlist[i].host.eip[j],"10.10.%d.%d",temp1,temp2);	//格式化ip地址

			/*检测ip地址是否有重复,若重复则重新生成*/
			//if(!ifiprepeat(i,j,0))	j--;
		}

		_gthis->sd->pmlist[i].host.stat=0;
		strcpy(_gthis->sd->pmlist[i].host.ip,_gthis->sd->pmlist[i].host.eip[0]);	//确定一个主ip地址

		/*生成cpu,异构:规格分为4,8,16,32c*/
		strcpy(_gthis->sd->pmlist[i].host.load[0].metric_name,"cpu");
		_gthis->sd->pmlist[i].host.load[0].used=0;
		_gthis->sd->pmlist[i].host.load[0].total=random_int_ab(random_int_ab(4,8),random_int_ab(16,32));

		/*生成mem,异构:规格规定为4G,8G,16G,32G,64G*/
		strcpy(_gthis->sd->pmlist[i].host.load[1].metric_name,"mem");
		_gthis->sd->pmlist[i].host.load[1].used=0;
		_gthis->sd->pmlist[i].host.load[1].total
			=random_int_ab(random_int_ab(8000,16000),random_int_ab(4000,32000));	

		/*生成一网络层面的网卡资源容量:单位：bps，规格如1GB,2GB,3GB,4GB*/
		for(d=0;d<PORT_NUM;d++){
			sprintf(eth,"eth%d",d);
			strcpy(_gthis->sd->pmlist[i].host.load[d].metric_name,eth);
			_gthis->sd->pmlist[i].host.load[d].used=0;
			_gthis->sd->pmlist[i].host.load[d].total=100;		
		}
		
		/*生成2网络层面的网卡资源容量*/
		/*strcpy(_gthis->sd->pmlist[i].host.load[3].metric_name,"eth1");
		_gthis->sd->pmlist[i].host.load[3].used=0;
		_gthis->sd->pmlist[i].host.load[3].total=100;
		*/
		/*生成3号网络层面的网卡资源容量*/
		/*strcpy(_gthis->sd->pmlist[i].host.load[4].metric_name,"eth2");
		_gthis->sd->pmlist[i].host.load[4].used=0;
		_gthis->sd->pmlist[i].host.load[4].total=100;
		*/
		/*生成4号网络层面的网卡资源容量*/
		/*strcpy(_gthis->sd->pmlist[i].host.load[5].metric_name,"eth3");
		_gthis->sd->pmlist[i].host.load[5].used=0;
		_gthis->sd->pmlist[i].host.load[5].total=100;
		*/
		
		/*生成磁盘IO，磁盘容量：GB*/
		strcpy(_gthis->sd->pmlist[i].host.load[6].metric_name,"diskio");
		_gthis->sd->pmlist[i].host.load[6].used=0;
		_gthis->sd->pmlist[i].host.load[6].total=100;

		
		/*生成FPGA*/
		strcpy(_gthis->sd->pmlist[i].host.load[7].metric_name,"fpga");
		_gthis->sd->pmlist[i].host.load[7].used=0;
		_gthis->sd->pmlist[i].host.load[7].total=100;

		if(i < hotpm)
			reserved = 0;		//保留量小，使物理机成为热点
		else if(i < coldpm+hotpm)
			reserved = random_a2b(HOT,1);//保留量大，使物理机成为冷点
		else
			reserved = random_a2b(COLD,HOT);

		for(j=0;j<DIM;j++){//设置保留量以保障物理机的冷热
			_gthis->sd->pmlist[i].host.load[j].reserved 
				= _gthis->sd->pmlist[i].host.load[j].total * reserved;//???
		}
	}

	return 1;
}
/*hcv:表示一个负载冷热程度的系数，0.8表示一个限额，不可以超过hcv，超过0.8*/
/*判断vm是否能放得下pm,放得下返回1，放不下返回0*/
int canPack(vmPtr p_vm, pmPtr p_pm, double hcv)
{
	int i,flag;
	double s,t;
	flag=1;
	for(i=0;i<DIM;i++)
	{
		s=p_pm->host.load[i].used+p_pm->host.load[i].reserved;
		s = s + p_vm->host.load[i].total;/*加入total表示放入*/
		t = p_pm->host.load[i].total;
		if(s > (t*hcv)){
			flag=0;break;
		}		
	}

	return flag;
}

/*更新vm到pm*/
void update_vm_to_pm(vmPtr p_vm, pmPtr p_pm)
{
	int i;
	double s;
	
	for(i=0;i<DIM;i++)
	{
		s=p_pm->host.load[i].used;
		s+=p_vm->host.load[i].total;/*加入total表示放入*/
		p_pm->host.load[i].used=s;
	}

	p_vm->at=p_vm->at_pmid=p_pm->host.id;
	
}



/**********************************************************************/
/*数据生成规则
按照VM个数循环
每生成一个VM，生成一个PM，将VM装入，
生成一组PM，一组VM；
参数：vmnum,VM个数  a,b属于[0,1),表示负载的范围
     int hotnum  >0.8,int coldnum <0.2
创建：陈建海
*/

void generate_data_by_vms(int vmnum, float a,float b,float hv)
{
	int i,j,k,ip3,ip4,flag;
	//int cp=1;/*1-表示要创建PM， 0-表示不用创建*/
	double rr,rp;	
	int tot_pm,tot_vm;
	vmPtr pv;pmPtr pp;st_vm vm;st_pm pm;
		
	tot_pm=_gthis->sd->pm_total;
	tot_vm=_gthis->sd->vm_total;

	
	i=0;j=0;
	/*以PV[0]*/
	while(i<vmnum){
		
		/*生成一台VM*/
		vm.host.id=tot_vm+i;
		sprintf(vm.host.sid,"vm%d",tot_vm+i);
		vm.host.stat=0;
		/*生成IP*/
		ip3=(int)(random_021()*255);
		ip4=(int)(random_021()*255);
		sprintf(vm.host.ip,"10.10.%d.%d",ip3,ip4);

		/*8个维度数据生成，以PMV模板进行生成*/
		for(k=0;k<DIM;k++){
			rr=a+(b-a)*random_021();/*随机数0-1,在a,b之间*/
			//strcpy(vm.host.load[k].metric_name, sh.getName(k));
			vm.host.load[k].total=minPMV.host.load[k].total*rr;
			vm.host.load[k].used=vm.host.load[k].total;
			vm.host.load[k].reserved=0;
		}

		/*加入VM到vmlist*/
		_gthis->sd->vmlist[tot_vm+i]=vm;
		
		pv=&_gthis->sd->vmlist[tot_vm+i];/*指向第1VM*/
		
		/*************************************************/
		/*寻找PM放入*/
		while(1){
			/*判断vm是否放得下pm，	在已经开启的PM中
			查找第一个可以放得下的PM，放入*/
			flag=0;
			for(k=tot_pm;k<tot_pm+j;k++){
				pp=&_gthis->sd->pmlist[k];
				if(canPack(pv,pp,hv)){
					flag=1;
					break;
				}
			}

			if(flag==1)/*表示已经找到可以放入的PM*/
			{
				/*放入vm，更新pm的used*/
				update_vm_to_pm(pv,pp);

				/*更新VM所在的PM*/
				pv->at_pmid=pp->host.id;
				pv->at=pv->at_pmid;/*默认at在PMID*/
				
				/*更新PM所包含的VM数组*/
				pp->p_vms[pp->countofvm]=pv;/*放入*/
				pp->countofvm++;/*vm个数增加1*/
				
				break;/*退出放置PM*/
			}
			else /*没有找到，新开PM*/
			{
				
				rp=4*random_021();/*给定的4个规格中选择一个作为模板复制生成新PM*/
				
				/*生成一个PM，将VM放入*/
				pm=PMV[(int)rp];
				
				pm.host.id=tot_pm+j;
				pm.host.stat=0;
				sprintf(pm.host.sid, "pm%d",tot_pm+j);

				/*生成IP*/
				ip3=(int)(random_021()*255);
				ip4=(int)(random_021()*255);
				sprintf(pm.host.ip,"10.10.%d.%d",ip3,ip4);
				pm.countofvm=0;
				
				/*将PM加入PMLIST*/
				_gthis->sd->pmlist[tot_pm+j]=pm;
				
				j++;/*PM下标增加1，准备生成下一个PM*/
				_gthis->sd->pm_total++;
			}
		}
		
		/*准备下一个VM*/
		i++;
		/*累计VM总数*/
		_gthis->sd->vm_total++;
	}
}

/***********************************************************************/
/*
* 函数名 : void create_empty_pm(int pmnum)
* 功能 : 随机创建虚拟机列表
* 传入参数 : int pmnum
* 参数 pmnum : 需要生成的PM数量
* 返回值 : 无
* @created by 陈建海 in 2013.7.26
* @modify by  in 2013.7.26
*/
void create_empty_pm(int pmnum)
{
	int i,rp,ip3,ip4;
	int tot_pm;
	st_pm pm;

	tot_pm=_gthis->sd->pm_total;
	
	createPMV();/*生成模板*/

	for(i=0;i<pmnum;i++){
		rp=(int)4*random_021();/*给定的4个规格中选择一个作为模板复制生成新PM*/
				
		/*生成一个PM，将VM放入*/
		pm=PMV[(int)rp];
				
		pm.host.id=tot_pm+i;
		pm.host.stat=0; /*空载PM*/
		sprintf(pm.host.sid, "pm%d",tot_pm+i);

		/*生成IP*/
		ip3=(int)(random_021()*255);
		ip4=(int)(random_021()*255);
		sprintf(pm.host.ip,"10.10.%d.%d",ip3,ip4);
		pm.countofvm=0;
				
		/*将PM加入PMLIST*/
		_gthis->sd->pmlist[tot_pm+i]=pm;
		
		_gthis->sd->pm_total++;
	}
}



/***********************************************************************/
/*
* 函数名 : void set_empty_pm()
* 功能 : 清空PM为空载
* 传入参数 : pmPtr pmlist 表示要置空载的PM集合
* 返回值 : 无
* @created by 陈建海 in 2013.7.26
* @modify by  in 2013.7.26
*/
void set_empty_pm(pmPtr pmlist,int num)
{
	int i,j,k;
	int tot_pm;

	pmPtr ppm;
	//tot_pm=_gthis->sd->pm_total;
	tot_pm=num;

	for(i=0;i<tot_pm;i++){
		ppm=&pmlist[i];//        &_gthis->sd->pmlist[i];	
		
		for(j=0;j<ppm->countofvm;j++)
			ppm->p_vms[j]=NULL;

		for(k=0;k<DIM;k++)
			ppm->host.load[k].used=0;

		ppm->host.stat=0;/*0-待放置状态*/

		ppm->countofvm=0;
	}
}



/***********************************************************************/
/*
* 函数名 : void createvm(double a,double b，int vmnum,int pmnum, int hotpm,int coldpm)
* 功能 : 随机创建虚拟机列表
* 传入参数 : double a, double b，int vmnum,int pmnum, int hotpm,int coldpm
* 参数a,b : 指标值的范围边界值，如cpu利用率的范围是0，1 或 0~32个核
* 参数vmnum, pmnum : 需要生成的VM\PM数量
* 参数int hotpm,int coldpm： 热点PM数量，冷点PM数量：温和WARM数量就是PM总数pmnum-hot-cold
* 返回值 : 无
* @created by 孙业成 in 2013.5.11
* @modify by sun in 2013.6.17
*/
void create_vm(double a,double b,int vmnum,int pmnum, int hotpm,int coldpm)
{
	int AT,k;
	int i;

	set_seed();		//获取随机数种子

	for(i=0;i<vmnum;i++){
		
		_gthis->sd->vmlist[i].host.id = i;//主机内码ID
		sprintf(_gthis->sd->vmlist[i].host.sid,"vm%d",i); //主机外码ID

		//生成IP地址
		createIP(DEFIP,_gthis->sd->vmlist[i].host.ip);

		//判断ip地址是否已经存在或重复,若重复则重新生成
		if(!ifiprepeat(i,0,1)){
			i--;
			continue;
		}
	
		if(i<hotpm+coldpm)
			AT = i; //保证前hotpm台热点及和前coldpm台冷点机没有空物理机
		else
			AT=_gthis->sd->vmlist[i].at = (int) (random_021()* pmnum);//随机生成宿主物理机

		for(k=0;k<DIM;k++){
			/*依次将8个维度的名字赋值*/
			strcpy(_gthis->sd->vmlist[i].host.load[k].metric_name,
				_gthis->sd->pmlist[AT].host.load[k].metric_name);	
		
			switch(k){
				case ETH0:case ETH1:case  ETH2: case ETH3:
					
					/*随机给每一块网卡生成负载,其余均为0*/
					_gthis->sd->vmlist[i].host.load[k].used 
						= _gthis->sd->vmlist[i].host.load[k].total 
						= (_gthis->sd->pmlist[AT].host.load[k].total 
						- _gthis->sd->pmlist[AT].host.load[k].reserved) * random_a2b(a,b);
					//_gthis->sd->vmlist[i].host.link_port = k - 2;//存储生成的网卡编号
			
					break;
				case CPU: case MEM: case FPGA: case DISKIO:

					_gthis->sd->vmlist[i].host.load[k].used
					= _gthis->sd->vmlist[i].host.load[k].total
					= (_gthis->sd->pmlist[AT].host.load[k].total 
						- _gthis->sd->pmlist[AT].host.load[k].reserved) * random_a2b(a,b);
					break;
			}
	
		}

//		k=(int)random_a2b(2,6);//其实是随机产生2-5

		
		if(!updatepm(i,AT)){
			//更新宿主物理机,并判断是否能容纳,若超出容量则换第二种方式生成
			//第一种方式见上,vm的负载=(pm.total-pm.reserved(保留量))*随机数
			//第二种方式见下,vm的负载=(pm的剩余空间-保留量)*随机数
			for(k=0;k<DIM;k++)
				
				if((k<2)|(k>5))
					_gthis->sd->vmlist[i].host.load[k].used
					= _gthis->sd->vmlist[i].host.load[k].total
					= (_gthis->sd->pmlist[AT].host.load[k].total - _gthis->sd->pmlist[AT].host.load[k].used
					- _gthis->sd->pmlist[AT].host.load[k].reserved ) * random_a2b(a,b);

			k=(int)random_a2b(2,6);

			_gthis->sd->vmlist[i].host.load[k].used 
				= _gthis->sd->vmlist[i].host.load[k].total 
				= (_gthis->sd->pmlist[AT].host.load[k].total - _gthis->sd->pmlist[AT].host.load[k].used
				- _gthis->sd->pmlist[AT].host.load[k].reserved ) * random_a2b(a,b);
			_gthis->sd->vmlist[i].host.link_port = k - 2;
			
			updatepm(i,AT);
		}
	}

}

/***********************************************************************/
/*将所有物理机的reserved置为0*/
void clear_res(void)
{
	int i,j;
	for(i=0;i<_gthis->sd->pm_total;i++)						//遍历所有物理机
		for(j=0;j<DIM;j++)
			_gthis->sd->pmlist[i].host.load[j].reserved = 0;
}


/***********************************************************************/
/*
* 函数名 : ifv2vrepeat(int src_id,int dst_id)
* 功能 : 检测随机生成的vm2vm是否重复
* 传入参数 : int i,int j, int ID
* 参数src_id : src_id即源vmID
* 参数dst_id : dst_id即目标vmID
* 参数ID : _gthis->sd->v2vlist
* 返回值 : return 1表示有重复, return 0表示无重复
* @created by 陈建海 in 2013.7.25
*/
int if_v2v_exist(int src_id,int dst_id,int afftyp)
{
	int i,flag;
	vm2vmPtr p_vv;

	if(afftyp <=0){
		/*与之前生成的流量关系依次比较,若有重复则return 0*/
		for (i=0; i < _gthis->sd->v2v_total-1; i++)
		{
			p_vv=&_gthis->sd->v2vlist[i];
			if(p_vv->affinity==afftyp)
			{
				if((p_vv->p_dst_vm->host.id == dst_id 
					&& p_vv->p_src_vm->host.id == src_id)
					||(p_vv->p_dst_vm->host.id == src_id 
					&& p_vv->p_src_vm->host.id == dst_id))
				{
					flag=1;break;
				}
			}
		}
	}
	else /*同板*/
	{
		/*同框同组同板*/
		/*同框：如果已经设置过同框，则不需要再设置，
		如"vm1,vm2,vm3,vm4",生成一个"vm1,vm2"，
		不可以，"vm2,vm3"，都不可以，但存在"vm1,vm2"反板亲和，反组亲和，
		是可以的
		同板：允许亲和，不允许不同板，不同框，不同组；
		*/
		/*与之前生成的流量关系依次比较,若有重复则return 0*/
		
		
		
		for (i=0; i < _gthis->sd->v2v_total-1; i++)
		{
			p_vv=&_gthis->sd->v2vlist[i];
			if(p_vv->affinity ==afftyp)
			{/*对当前产生的亲和对，检查是否已经存在*/
				
				{
					flag=1;break;
				}
			}
		}
		
	}

	return flag;
}


int conflictbyvms(vmPtr* pp_vm,int aftype)
{
		
	return 0;
}

/*
 *加入一组VM到v2vlist:同板、同框、同组
 *pv是指针，指向一个数组，num是个数
 *创建者：陈建海，2013-07-26
 */
void addtov2vlistbyvms(vmPtr *pv,int num,int afftype)
{
	int i,tot_vv;
	char vmids[200];
	vm2vmPtr pvv;
	vmPtr pvm;
	tot_vv=_gthis->sd->v2v_total;

	/*在v2vlist末尾新增加一个V2V下标用tot_vv*/
	pvv=&_gthis->sd->v2vlist[tot_vv];
	pvv->id=tot_vv;
	pvv->countofvm=num;
	pvv->affinity=afftype;
	
	/*生成vmids*/
	for(i=0;i<num;i++)
	{
		pvm=pv[i];
		if(i==0)
			sprintf(vmids,"%s",pvm->host.sid);
		else
			sprintf(vmids,"%s,%s",vmids,pvm->host.sid);
		
		pvv->p_vms[i]=pvm;
	}
	strcpy(pvv->vmids,vmids);
	pvv->delay=0;
	pvv->traffic=0;
	pvv->p_src_vm=pv[0];
	pvv->p_dst_vm=pv[1];

	/*放入V2Vlist*/
	tot_vv++;
	_gthis->sd->v2v_total=tot_vv;	
}

/********************************************************
不同组反亲和:-3，在不同组中找两个VM设置为不同组亲和
随机取两个不同组，再在每个组随机选取一个PM，然后分别
选择两个不同PM中的VM，建立反亲和关系
fff: 1-随机生成； 0-指定生成
--------------------------------------------------------*/
void createOnev2v_3(int fff)
{
	int vmc1,vmc2,tot_pm,tot_gr,i,j;
	int r1,r2,p1,p2,f1,f2,flag;
	pmPtr p_pm1,p_pm2;
	vmPtr p_vm1,p_vm2;
	pmgPtr p_gr1,p_gr2;
	

	if(fff==1)
	{
		/*1、随机选择两个不同的组*/
		tot_gr=_gthis->sd->gr_total;
		f1=f2=0;
		while(f1==f2){
			f1=(int)(tot_gr*random_021()); 		
			f2=(int)(tot_gr*random_021()); 		
		}
	
		p_gr1=&_gthis->sd->pmglist[f1];
		p_gr2=&_gthis->sd->pmglist[f2];
		/*---------------------------------------------*/

		/*2.从每个组随机选择PM*/
	ppp1:
		tot_pm=p_gr1->countofpm;
		p1=p_gr1->i_pmids[(int)(tot_pm*random_021())];
		p_pm1=&_gthis->sd->pmlist[p1];
		if(p_pm1->countofvm-p_pm1->cost==0) goto ppp1;

	ppp2:
		tot_pm=p_gr2->countofpm;
		p2=p_gr2->i_pmids[(int)(tot_pm*random_021())];
		p_pm2=&_gthis->sd->pmlist[p2];
		if(p_pm2->countofvm-p_pm2->cost==0) goto ppp2;
	
		/*3.从每个PM中随机选择一个VM*/
		flag=0;
		while(flag==0){
			vmc1=p_pm1->countofvm;
			r1=(int)(vmc1*random_021());
			p_vm1=p_pm1->p_vms[r1];
		
			vmc2=p_pm2->countofvm;
			r2=(int)(vmc2*random_021());
			p_vm2=p_pm2->p_vms[r2];
	
			/*是否存在*/
			if(conflict(p_vm1->host.id,p_vm2->host.id,-3)==1)	{ 
				flag=0;/*存在了需要重新生成*/
			}
			else flag=1;
		}
	}
	if(fff==0)
	{
		/*选取第1个组号,选取1个PM，选取一个VM*/
		tot_gr=_gthis->sd->gr_total;
gg1:
		printf("输入第1个组号0-%d",tot_gr-1);
		scanf("%d",&f1);
		p_gr1=&_gthis->sd->pmglist[f1];
		if(p_gr1->countofpm==0) goto gg1;
		
gg2:
		printf("输入第2个组号0-%d",tot_gr-1);
		scanf("%d",&f2);
		if(f1==f2) {printf("第2个组号与第1个组号不能相同，请重新输入："); goto gg2;}
		p_gr2=&_gthis->sd->pmglist[f2];
		if(p_gr2->countofpm==0) goto gg2;
		/*2.从每个组输入要选的PM号*/

ppp3:
		tot_pm=p_gr1->countofpm;
		
		printf("输入组1的第1个PM号：\n PM号[VM数量]\n");
		for(i=0;i<tot_pm;i++)
		{
			p1=p_gr1->i_pmids[i];
			p_pm1=&_gthis->sd->pmlist[p1];
			
			if(i==0) printf("%d[%d]",p1,p_pm1->countofvm);
			else printf(",%d[%d]",p1,p_pm1->countofvm);
		}
		printf("\n");
		/*输入第一个PM号与VM序号*/
		scanf("%d",&p1);
		p_pm1=&_gthis->sd->pmlist[p1];
		/*显示该PM下的所有VMID，选取一个VM*/
		for(i=0;i<p_pm1->countofvm;i++)
		{
			if(i==0) printf("%d[%s]",i,p_pm1->p_vms[i]->host.sid);
			else printf(",%d[%s]",i,p_pm1->p_vms[i]->host.sid);
		}
		printf("\n输入所选VM的序号：0-%d",p_pm1->countofvm);
		scanf("%d",&r1);
		p_vm1=p_pm1->p_vms[r1];
		
ppp4:
		tot_pm=p_gr2->countofpm;
		
		printf("输入第2个PM号来自组2：\n PM号[VM数量]");
		for(i=0;i<tot_pm;i++)
		{
			p2=p_gr2->i_pmids[i];
			p_pm2=&_gthis->sd->pmlist[p2];
			
			if(i==0) printf("%d[%d]",p2,p_pm2->countofvm);
			else printf(",%d[%d]",p2,p_pm2->countofvm);
		}
		/*输入第二个PM号与VM序号*/
		scanf("%d",&p2);
		p_pm2=&_gthis->sd->pmlist[p2];
		/*显示该PM下的所有VMID，选取一个VM*/
		for(i=0;i<p_pm2->countofvm;i++)
		{
			if(i==0) printf("%d[%s]",i,p_pm2->p_vms[i]->host.sid);
			else printf(",%d[%s]",i,p_pm2->p_vms[i]->host.sid);
		}
		printf("\n输入所选VM的序号：");
		scanf("%d",&r2);
		p_vm2=p_pm2->p_vms[r2];
				
	}
	/*获取两个VM：p_vm1,p_vm2*/
	/*创建一条亲和边，加入到v2vlist*/
	/*加入到v2vlist末尾*/
	addtov2vlist(p_vm1,p_vm2,0,0,-3);
}

/********************************************************
不同框反亲和:-2，在不同框中找两个VM设置为不同框亲和
随机取两个不同框，再在每个框随机选取一个PM，然后分别
选择两个不同的VM，建立反亲和关系
--------------------------------------------------------*/
void createOnev2v_2()
{
	int vmc1,vmc2,tot_pm,tot_fr;
	int r1,r2,p1,p2,f1,f2,flag;
	pmPtr p_pm1,p_pm2;
	vmPtr p_vm1,p_vm2;
	framePtr p_fr1,p_fr2;
	
	/*1、随机选择两个不同的框*/
	tot_fr=_gthis->sd->fr_total;
	f1=f2=0;
	while(f1==f2){
		f1=(int)(tot_fr*random_021()); 		
		f2=(int)(tot_fr*random_021()); 		
	}
					
	p_fr1=&_gthis->sd->framelist[f1];
	p_fr2=&_gthis->sd->framelist[f2];
	/*---------------------------------------------*/

	/*2.从每个框随机选择PM*/
	tot_pm=p_fr1->countofpm;
	p1=p_fr1->i_pmids[(int)(tot_pm*random_021())];

	tot_pm=p_fr2->countofpm;
	p2=p_fr2->i_pmids[(int)(tot_pm*random_021())];

	p_pm1=&_gthis->sd->pmlist[p1];
	p_pm2=&_gthis->sd->pmlist[p2];

	/*3.从每个PM中随机选择一个VM*/
	flag=0;
	while(flag==0){
		vmc1=p_pm1->countofvm;
		r1=(int)(vmc1*random_021());
		p_vm1=p_pm1->p_vms[r1];
	
		vmc2=p_pm2->countofvm;
		r2=(int)(vmc2*random_021());
		p_vm2=p_pm2->p_vms[r2];
					
		/*是否存在*/
		if(conflict(p_vm1->host.id,p_vm2->host.id,-2)==1)	{ 
			flag=0;/*存在了需要重新生成*/
		}
		else flag=1;
	}

	/*获取两个VM：p_vm1,p_vm2*/
	/*创建一条亲和边，加入到v2vlist*/
	/*加入到v2vlist末尾*/
	addtov2vlist(p_vm1,p_vm2,0,0,-2);

}



/********************************************************
不同板反亲和:-1，在不同一个PM中找两个VM设置为同板或流量亲和
      随机选取两个PM，分别选择两个不同的VM，建立反亲和关系
--------------------------------------------------------*/
void createOnev2v_1(int afftyp)
{
	int vmc1,vmc2,tot_pm;
	int r,r0,r1,r2,flag;
	pmPtr p_pm1,p_pm2;
	vmPtr p_vm1,p_vm2;
	
	/*流量:在两个不同PM中找两个VM设置为同板反亲和*/
	
	/*找第一个*/
	vmc1=0;
	while(vmc1<1){ 
		/*随机找一个PM*/
		tot_pm=_gthis->sd->pm_total;
		r=(int)(tot_pm*random_021());
		p_pm1=&_gthis->sd->pmlist[r];

		/*获取该PM中的VMS数量*/
	   	/*如果数量不足两个，则换一个PM继续*/
		vmc1=p_pm1->countofvm;
	}
	
	/*找第一个*/
	vmc2=0;
	r0=r;
	while(vmc2<1 || r0==r){ 
		/*随机找一个PM*/
		tot_pm=_gthis->sd->pm_total;
		r0=(int)(tot_pm*random_021());
		p_pm2=&_gthis->sd->pmlist[r0];
		/*获取该PM中的VMS数量*/
	   	/*如果数量不足1个，则换一个PM继续*/
		vmc2=p_pm2->countofvm;
	}
	

	/*准备两个随机数*/	
	r1=r2=0;				
	/*VM数>=2*/
	flag=0;
	while(flag==0){				
		r1=(int)(vmc1*random_021());
		r2=(int)(vmc2*random_021());
					
		/*获取两个VM*/
		p_vm1=p_pm1->p_vms[r1];
		p_vm2=p_pm2->p_vms[r2];;
					
		/*是否存在*/
		if(conflict(p_vm1->host.id,p_vm2->host.id,-1)==1)	{ 
			flag=0;/*存在了需要重新生成*/
		}
		else
			flag=1;
	}


	/*获取两个VM：p_vm1,p_vm2*/
	/*创建一条亲和边，加入到v2vlist*/
	/*加入到v2vlist末尾*/
	addtov2vlist(p_vm1,p_vm2,0,0,-1);

}




/*流量:在同一个PM中找两个VM设置为流量亲和*/
void createOnev2v0(float a,float b)
{
	int vmcount,tot_pm;
	int r,r1,r2,flag;
	pmPtr p_pm;
	vmPtr p_vm1,p_vm2;

	/*流量:在同一个PM中找两个VM设置为同板或流量亲和*/
	vmcount=0;

	while(vmcount<4){ 
		/*随机找一个PM*/
		tot_pm=_gthis->sd->pm_total;
		r=(int)(tot_pm*random_021());
		p_pm=&_gthis->sd->pmlist[r];

		/*获取该PM中的VMS数量*/
	   	/*如果数量不足两个，则换一个PM继续*/
		vmcount=p_pm->countofvm;
	}
	
	/*准备两个随机数*/	
	r1=r2=0;				
	/*VM数>=2*/
	flag=0;
	while(flag==0||r1==r2){				
		r1=(int)(vmcount*random_021());
		r2=(int)(vmcount*random_021());
					
		/*获取两个VM*/
		p_vm1=p_pm->p_vms[r1];
		p_vm2=p_pm->p_vms[r2];;
					
		/*是否存在*/
		if(conflict(p_vm1->host.id,p_vm2->host.id,0)==1)	{ 
			flag=0;/*存在了需要重新生成*/
		}
		else
			flag=1;
	}


	/*获取两个VM：p_vm1,p_vm2*/
	/*创建一条亲和边，加入到v2vlist*/
	/*加入到v2vlist末尾*/
	addtov2vlist(p_vm1,p_vm2,a,b,0);

}


/********************************************************
同板亲和:1，在同一个PM中找若干个VM设置为同板亲和
      个数随机产生，随机选取PM，分别选择VM，建立同板亲和关系
--------------------------------------------------------*/
void createOnev2v1()
{
	int vmcount,vmcc,tot_pm;
	int r,r1,flag,i,j;

	pmPtr p_pm,pmlst[4000];
	vmPtr pvm,p_vm[20];/*同板最多不超过20个*/


	/*同板,随机产生同板VM的个数>=2*/
	tot_pm=_gthis->sd->pm_total;
	/*找出VM个数大于2的PM*/
	for(i=0,j=0;i<tot_pm;i++)
	{
		p_pm=&_gthis->sd->pmlist[i];
		if((p_pm->countofvm-(int)p_pm->cost)>=2)
			pmlst[j++]=p_pm;
	}
	
	if(j==0){
		printf("No pm can create the same group affinity!\n");
		return;
	}
	
	vmcount=0;
	while(vmcount<3){
		r=(int)(j*random_021());
		p_pm=&_gthis->sd->pmlist[r];
		/*获取该PM中的VMS*/
		vmcount=p_pm->countofvm-p_pm->cost;
	}

	vmcc=0;/*个数*/
	while(vmcc<2||vmcc>=20){
		vmcc=(int)(vmcount*random_021());					
	}
	
	if(vmcc>=vmcount) vmcc=vmcount;

	/*获得vmcc个同板的VM*/
	j=0;
	while(j<vmcc)
	{
		flag=0;
		while(flag==0)
		{
			/*获得一个VM*/
			r1=(int)(vmcount*random_021());/*随机获取一个*/
			pvm=p_pm->p_vms[r1];
			/*判断pvm是否已经存在*/
			flag=1;
			for(i=0;i<j;i++)
			{
				if(pvm->host.id==p_vm[i]->host.id) 
				{
					flag=0;
					break;
				}
			}
		}
		p_vm[j]=pvm;
		pvm->ask=1;
		j++;
	}

	

	/*加入v2vlist*/
	addtov2vlistbyvms(p_vm,vmcc,1);
}


/********************************************************
同框亲和:1，在同一个框中找若干个PM，再找若干VM设置为同框亲和
      个数随机产生，随机选取PM，分别选择VM，建立同框亲和关系
--------------------------------------------------------*/
void createOnev2v2()
{
	int j,k,flag,tot_pm,tot_fr,vmcc;
	int r;
	framePtr p_fr1;
	vmPtr p_vm,p_vms[20]; /*同框的VM个数不超过50*/
	pmPtr p_pm;
	
	/*获取一个框*/
	tot_fr=_gthis->sd->fr_total;
	p_fr1=&_gthis->sd->framelist[(int)(tot_fr*random_021())];

	/*选择PM*/
	tot_pm=p_fr1->countofpm;
		
	/*生成要求同框的VM个数*/
	vmcc=0;
	while(vmcc<2||vmcc>=20)/*至少2个*/
	{
		vmcc=(int)(20*random_021());					
	}


	j=0;k=0;
	while(j<vmcc||flag==1)
	{
		/*获得一个PM*/
		r=p_fr1->i_pmids[(int)(tot_pm*random_021())];
		p_pm=&_gthis->sd->pmlist[r];

		/*随机获得一个vm*/
		p_vm=p_pm->p_vms[(int)(p_pm->countofvm*random_021())];

		/*是否已经存在*/
		flag=0;
		for(k=0;k<j;k++){
			if(p_vm->host.id==p_vms[k]->host.id){
				flag=1;break;
			}
		}

		if(flag==0)
			p_vms[j++]=p_vm;

		/*循环找20000以上还没找到，则强制退出*/
		if(k>20000) break; 
		k++;
	}

	/*加入到v2vlist*/
	if(k<20000)
		addtov2vlistbyvms(p_vms,vmcc,2);

}


/********************************************************
同组亲和:1，在同一个组中找若干个PM，再找若干VM设置为同组亲和
      个数随机产生，随机选取PM，分别选择VM，建立同组亲和关系
--------------------------------------------------------*/
int createOnev2v3()
{
	int j,k,flag,tot_pm,tot_gr,vmcc;
	int r,r1,r2;
	pmgPtr p_gr1;
	vmPtr p_vm,p_vms[20]; /*同框的VM个数不超过50*/
	pmPtr p_pm;

	/*获取一个组*/
	tot_gr=_gthis->sd->gr_total;
	p_gr1=&_gthis->sd->pmglist[(int)(tot_gr*random_021())];

	
	/*选择PM*/
	tot_pm=p_gr1->countofpm;
		
	/*生成要求同组的VM个数*/
	vmcc=0;
	while(vmcc<2||vmcc>=20)/*至少2个*/
	{
		vmcc=(int)(20*random_021());					
	}

	j=0; k=0;
	while(j<vmcc||flag==1)
	{
		/*获得一个PM*/
		r1=(int)(tot_pm*random_021());
		r=p_gr1->i_pmids[r1];
		p_pm=&_gthis->sd->pmlist[r];

		/*随机获得一个vm*/
		r2=(int)(p_pm->countofvm*random_021());
		p_vm=p_pm->p_vms[r2];

		/*是否已经存在*/
		flag=0;
		for(k=0;k<j;k++){
			if(p_vm->host.id==p_vms[k]->host.id){
				flag=1;break;
			}
		}

		if(flag==0)
			p_vms[j++]=p_vm;
		
		k++;
		if(k>20000) break;
	}


	/*加入到v2vlist*/
	if(k<20000){
		addtov2vlistbyvms(p_vms,vmcc,3);
		return 1;
	}
	else
		return 0;
}

/*加入到v2vlist*/
void addtov2vlist(vmPtr p_vm1,vmPtr p_vm2,float a,float b,int aftype)
{
	int tot_v2v;
	vm2vmPtr p_vv;

	tot_v2v=_gthis->sd->v2v_total;
	p_vv=&_gthis->sd->v2vlist[tot_v2v];
	
	/*更新v2v*/
	p_vv->id=tot_v2v;
	p_vv->traffic=(aftype==0)?a+(b-a)*random_021():0; 	
	p_vv->affinity=aftype;
	p_vv->delay=0;
	
	if(aftype<=0)/*两个反亲和与流量，VM是成对的，直接更新v2v*/
	{		
		/*流量*/
		p_vv->p_src_vm=p_vm1;
		p_vv->p_dst_vm=p_vm2;
		p_vv->countofvm=2;
		p_vv->p_vms[0]=p_vm1;
		p_vv->p_vms[1]=p_vm2;
		sprintf(p_vv->vmids,"%s,%s",p_vm1->host.sid,p_vm2->host.sid);
		if(DEBUG) printf("%s\n",p_vv->vmids);
	}

	/*放入V2Vlist*/
	tot_v2v++;
	_gthis->sd->v2v_total=tot_v2v;	
}



/*功能：流量亲和，同板亲和
函数参数：float a,float b, 流量大小的范围
          int affnum,   亲和边的个数
		  int aftype    亲和类型
创建者：CJH 2013.07.30
*/
void create_v2v_by_ta(float a,float b,int affnum,int aftype)
{
	int tot_v2v,tot_pm,tot_fr,tot_gr,i,fff,sel;
	
	srand( (unsigned)time( NULL ) ); 

	/*获得亲和边的总数*/
	tot_v2v=_gthis->sd->v2v_total;

	/*选两VM*/
	switch(aftype){
		case 0:
			/*逐边生成流量关系：在同一PM上的两个VM之间建立流量关系*/
			printf("选择：\n -1- 随机批量生成.\n");
			printf(" -2- 输入VMID生成.\n");
			scanf("%d",&sel);
			if(sel==1)
				for(i=0;i<affnum;i++) createOnev2v0(a,b);
			else
			{
				printf("输入两个VMID生成流量亲和边：");

			}

			break;
		case -3:/*不同组：
			在两个不同组中选取两个PM，然后分别从中选取两个VM*/
			/*如果不存在两个不同组的PM上的VM，则不能建立不同组亲和关系*/
			tot_gr=_gthis->sd->gr_total;
			
			if(tot_gr<2)
				printf("本数据的组不足2个，无法设置不同组亲和！\n");
			else{
				/*逐边生成不同组亲和关系*/
				printf("1-随机生成，0-手工输入");
				scanf("%d",&fff);
				for(i=0;i<affnum;i++) createOnev2v_3(fff);				
			}
			break;
		case -2:/*不同框*/
			/*在两个不同框的PM中找两个VM设置为不同框反亲和*/
			tot_fr=_gthis->sd->fr_total;
			
			if(tot_fr<2)
				printf("本数据的框不足2个，无法设置不同框亲和！\n");
			else
				/*逐边生成不同框关系*/
				for(i=0;i<affnum;i++) createOnev2v_2();				
		
			break;
		case -1:/*不同板*/
			/*逐边生成流量关系*/
			tot_pm=_gthis->sd->pm_total;
			if(tot_pm<2)
				printf("本数据的PM不足2个，无法设置不同板亲和！\n");
			else
				for(i=0;i<affnum;i++)	createOnev2v_1(-1);

			break;
		case 1:/*同板：任意选取一个PM上，取若干VM*/
			for(i=0;i<affnum;i++) createOnev2v1();

			break;
		case 2:/*同框:任意选择若干个PM，可以是同PM*/
			for(i=0;i<affnum;i++) createOnev2v2();
			
			break;
		case 3:/*同组*/
			for(i=0;i<affnum;i++) createOnev2v3();

			break;
	}
}

/***********************************************************************/
/*
* 函数名 : ifv2vrepeat(int src_id,int dst_id,int ID)
* 功能 : 检测随机生成的vm2vm是否重复
* 传入参数 : int i,int j, int ID
* 参数src_id : src_id即源vmID
* 参数dst_id : dst_id即目标vmID
* 参数ID : _gthis->sd->v2vlist
* 返回值 : return 0表示有重复, return 1表示无重复
* @created by 孙业成 in 2013.5.12
*/
int ifv2vrepeat(int src_id,int dst_id, int ID)
{
	int i;
	/*与之前生成的流量关系依次比较,若有重复则return 0*/
	for (i=0; i < ID-1; i++)
		if(_gthis->sd->v2vlist[i].p_dst_vm->host.id == dst_id 
			&& _gthis->sd->v2vlist[i].p_src_vm->host.id == src_id)
			return 0;

	return 1;
}

/*
* 函数名 : create_v2v_by_aff_type(float a, float b, int affnum, int afftype)
* 功能 : 随机生成vm流量关系
* 传入参数 :float a, float b, int affnum, int afftype
* 参数lower : 流量下限
* 参数upper : 流量上限
* 参数affnum : 生成边的数量
* 参数 afftype:亲和类型号 -3~3
* 返回值 : 无
* @created by 陈建海 in 2013.7.25
*/
void create_v2v_by_aff_type(float a, float b, int affnum, int afftype)
{
	/*3.亲和性*/
	/*printf("创建亲和性关系：\n");
	printf("0：-流量亲和性：随机选取PM，在其上的VM之间创建；\n");
	printf("-3：-反组亲和：在不同组中随机选取两个PM，在其上的VM之间创建；\n");
	printf("-2：-反框亲和：在不同框中随机选取两个PM，在其上的VM之间创建；\n");
	printf("-1：-反板亲和：随机选取两个PM，在其上的VM之间创建；\n");
	printf("1：-同组亲和：随机选取一个组，组中随机选取两个PM，在其上的VM之间创建；\n");
	printf("2：-同框亲和：随机选一个框，框中随机选取两个PM，在其上的VM之间创建；\n");
	printf("3：-同板亲和：随机选取一个PM，在其上的VM之间创建；\n");

	printf("请输入创建的亲和类型编号，及要创建的数量,如0 10，表示创建流量亲和性10条边。\n输入其它数则退出。\n");
	*/
	//scanf("%d%d",&choice,&afnum);

	
	if(afftype==0)	{
		create_v2v_by_ta(a,b,affnum,afftype);
	}
	else
		create_v2v_by_ta(0,0,affnum,afftype);

}


/***********************************************************************/
/*
* 函数名 : create_vm2vm (double lower, double upper, double total)
* 功能 : 随机生成vm流量关系
* 传入参数 : double lower, double upper, double total
* 参数lower : 流量下限
* 参数upper : 流量上限
* 参数total : 生成边的数量
* 返回值 : 无
* @created by 孙业成 in 2013.5.12
*/
void create_v2v(double lower, double upper, int vmnum, double ratio)
{
	int src_id, dst_id,t_id;
	int total_side;	//总边数
	int i;
	char vmids[500];

	total_side = (int)(vmnum * ratio);
	_gthis->sd->v2v_total = total_side;
	for (i = 0; i < total_side; i++){	//生成total_side条边
		src_id = (int) random_a2b(0,vmnum+1);//随机生成id
		dst_id = (int) random_a2b(0,vmnum+1);

		if (src_id == dst_id) dst_id++;

		if (src_id > dst_id) {//小的ID放在src_id,交换,edited by cjh 20130713
			t_id=src_id;
			src_id=dst_id;
			dst_id=t_id;
		}

		if (!ifv2vrepeat(src_id, dst_id, i)) i--;	//判断边是否已经存在

		_gthis->sd->v2vlist[i].id = i;/*数组下标序号*/

		_gthis->sd->v2vlist[i].p_vms[0]=&_gthis->sd->vmlist[src_id];
		_gthis->sd->v2vlist[i].p_vms[1]=&_gthis->sd->vmlist[dst_id];

		_gthis->sd->v2vlist[i].p_dst_vm = &_gthis->sd->vmlist[dst_id];
		_gthis->sd->v2vlist[i].p_src_vm = &_gthis->sd->vmlist[src_id];
		_gthis->sd->v2vlist[i].traffic = random_a2b(lower, upper);//随机生成流量（lower,upper）
		_gthis->sd->v2vlist[i].delay = (int)random_a2b(1,7);	//产生1-6内的随机数，交换机节点的个数
		
		strcpy(vmids,_gthis->sd->v2vlist[i].p_vms[0]->host.sid); 
		strcat(vmids,",");
		strcat(vmids,_gthis->sd->v2vlist[i].p_vms[1]->host.sid); 
		strcpy(_gthis->sd->v2vlist[i].vmids,vmids);

		/*生成流量亲和性类型 added by cjh 07/13/2013*/
		_gthis->sd->v2vlist[i].affinity =0; 

	}
}


/*检查亲和性是否冲突*/
int conflict(int vmid1,int vmid2,int afftyp){
	int i,j,k,flag,tid,id1,id2;

	if(vmid1>vmid2){
		tid=vmid1;vmid1=vmid2;vmid2=tid;
	}

	flag=0;
	for(i=0;i<_gthis->sd->v2v_total;i++)
	{
		// "1,2,3,4"
		if(_gthis->sd->v2vlist[i].affinity!=afftyp)
			continue;

		for(j=0;j<_gthis->sd->v2vlist[i].countofvm;j++)
		{
			id1=_gthis->sd->v2vlist[i].p_vms[j]->host.id;
			for(k=0;k<_gthis->sd->v2vlist[i].countofvm;k++)
			{
				if(k==j) continue;

				id2=_gthis->sd->v2vlist[i].p_vms[k]->host.id;
				
				if((vmid1==id1 && vmid2==id2) || (vmid1==id2 && vmid2==id1))
				{
					flag=1;break;
				}
			}
			if(flag==1) break;
		}
		if(flag==1) break;				
	}

	return flag;
}


/***********************************************************************/
/*
* 函数名 : void create_v2v_no_aff(int affnum，int afftype)
* 功能 : 随机生成vm反亲和关系, 
* 规则：同板-一定同框、同组，不会存在不同板、不同框、不同组；
*	   不同板-不可以同板，但可以同组，可以同框；
*      同框-可以同组，可以不同组，可以同板，可以不同板，不可以不同框；
*      同组-可以同框，可以不同框，可以同板，可以不同板，不可 不同组；
* 传入参数 : int vmnum, int affnum
* 参数affnum: 生成的组数
* 参数afftype: 生成亲和类型-3，-2，-1，1,2,3
* 返回值 : 无
* @created by 孙业成 in 2013.5.12
*/
void create_v2v_no_aff(int affnum, int afftype)
{
	int src_id, dst_id,t_id,vmnum,n;
	int v2v_tot=0;	//总边数
	int i,j,k,flag;
	int vmid[100];/*每个亲和组不可以超过100个id*/
	char vmids[500];
	vmnum=_gthis->sd->vm_total;
	v2v_tot=_gthis->sd->v2v_total;
	
	/*生成total_side条边*/
	for (i = v2v_tot; i < v2v_tot + affnum; i++){	
		if(afftype==-1 || afftype==-2 || afftype==-3){/*不同板、框、组*/
			src_id = (int) random_a2b(0,vmnum+1);//随机生成id
			dst_id = (int) random_a2b(0,vmnum+1);
		
			if (src_id == dst_id) dst_id++;

			if (src_id > dst_id) {//小的ID放在src_id,交换,edited by cjh 20130713
				t_id=src_id;
				src_id=dst_id;
				dst_id=t_id;
			}

			if (!ifv2vrepeat(src_id, dst_id, i)) i--;	//判断边是否已经存在
		
			
			/*生成亲和性类型 added by cjh 07/23/2013*/
			_gthis->sd->v2vlist[i].affinity = afftype;
		
			_gthis->sd->v2vlist[i].id = i;/*数组下标序号*/
		
			_gthis->sd->v2vlist[i].p_vms[0]=&_gthis->sd->vmlist[src_id];
			_gthis->sd->v2vlist[i].p_vms[1]=&_gthis->sd->vmlist[dst_id];

			_gthis->sd->v2vlist[i].p_dst_vm = &_gthis->sd->vmlist[dst_id];
			_gthis->sd->v2vlist[i].p_src_vm = &_gthis->sd->vmlist[src_id];
		
			/*不生成流量（lower,upper）*/
			_gthis->sd->v2vlist[i].traffic =0;
		
			/*产生1-6内的随机数，交换机节点的个数*/
			_gthis->sd->v2vlist[i].delay =0;	
		
			strcpy(vmids,_gthis->sd->v2vlist[i].p_vms[0]->host.sid); 
			strcpy(vmids,_gthis->sd->v2vlist[i].p_vms[1]->host.sid); 
			strcpy(_gthis->sd->v2vlist[i].vmids,vmids);

		} 
		else if(afftype==1) /*同板*/
		{/*同板、同框、同组亲和*/
			printf("输入同板vmids中vmid的个数：");
			scanf("%d",&n);

			j=0;
			while(j<n){
				vmid[j]=(int) random_a2b(0,vmnum+1);
				
				if(j>0){
					/*判断是否已经生成过了*/
					flag=0;
					for(k=0;k<j;k++)
						if(vmid[k]==vmid[j]) {flag=1;break;}

					if(flag==1) {continue;} /*有重复*/
					
					/*是否存在反亲和：当前vmid与前面已经生成vmid构成的同板亲和，
					是否存在反亲和，如果有，则出现冲突，需要重新生成vmid*/
					flag=0;
					for(k=0;k<j-1;k++)
					{
						if(conflict(vmid[k],vmid[j],afftype)){
							flag=1;break;
						}
					}

					if(flag==1) {continue;}
				}
				
				j++;
			}
		}

		/*总数+1*/
		_gthis->sd->v2v_total++;
	}
}



/*函数名：int getswnopmbypmid(int pmid)
* 功能：判断是否放置过
* 传入参数：int pmid
* 返回：   可以放置的交换机id
* 创建： 陈建海 7.29
*/
swPtr getswnopmbypmid(int pmid)
{
	int i,j,flag,tot_sw;
	swPtr p_sw;
	tot_sw=_gthis->sd->sw_total;
	
	for(i=0;i<tot_sw;i++){
		p_sw=&_gthis->sd->swlist[i];
		if(p_sw->countoflinks==p_sw->portnum) continue;
		
		flag=0;
		for(j=0;j<p_sw->countoflinks;j++)
		{
			if(p_sw->links[j].id==pmid && p_sw->links[j].type==PMLINK)
			{
				flag=1; 
				break;
			}
		}
		if(flag==0) return p_sw; 
	}

	return NULL;

}




/*
* 函数名 : create_pm_topo(void)
* 功能 : 随机生成物理拓扑，根据PMnum,确定所需的交换机个数(交换机端口数规定为24个)
* 传入参数 : double lower, double upper,int pmnum,int port
* 参数1 ：时延范围，pmnum-物理机台数，port:端口数 
* 返回值 : 无
* @created by 陈建海 in 2013.7.28
*/
void create_p2p_tree_topo(double lower, double upper,int pmnum)
{
	/*根据PMnum,每个PM包含4个端口确定所需的交换机个数*/
	int i,j,k,ports,pport,level=0,start_swid;
	int tot_sw,tot_s2s,tot_p2s;
	int swnum0,swnum1; /*0-2各层交换机个数*/
	pmPtr p_pm;
	swPtr psw,p_sw,p_sw1;
	pm2swPtr p_ps;
	sw2swPtr p_ss;
	st_swlink swlnk;
	
	/*---生成物理机端口与交换机0层端口的连接-----------*/
	
	//tot_pm=_gthis->sd->pm_total;
	tot_p2s=_gthis->sd->p2s_total;

	/*每个端口连入一个交换机*/
	j=0;k=0;level=0;
	while(j<pmnum){
		/*取第j个PM*/
		p_pm=&_gthis->sd->pmlist[j];
		
		/*为pm的每个端口建立一个与交换机端口的连接，每个PM端口需要连入不同交换机的端口*/
		/*随机生成该PM的端口数>=1,<=4*/
		ports=(int)(1+4*random_021());

		/*每个端口需要一个交换机接入*/
		for(pport=0;pport<ports;pport++)
		{			
			/*生成一条p2s连接*/
			p_ps=&_gthis->sd->p2slist[tot_p2s]; 
			
			/*PM端*/
			//p_pm->host.id=j;
			p_ps->id=tot_p2s;
			
			p_ps->pm_id=j;
			swlnk.id=j;
			swlnk.port=pport;
			swlnk.type=PMLINK;
			sprintf(p_ps->pm_sid,"pm%d",j);
			p_ps->port_id=pport;/*端口号*/
			
			/*switch端 一个端口只能接一个交换机*/
			/*找一个没有放过pmid的交换机*/
			psw=getswnopmbypmid(j);
			if(psw==NULL) /*则新开一个交换机*/
			{	tot_sw=_gthis->sd->sw_total;
				p_sw=&_gthis->sd->swlist[tot_sw];
				p_sw->id=tot_sw;
				p_sw->portnum=SW_PORT_NUM;/*端口个数范围*/
				p_sw->countoflinks=0;
				p_sw->level=level;
				sprintf(p_sw->sid,"sw%d",tot_sw);
		
				/*交换机总数增加1*/
				tot_sw++;
				_gthis->sd->sw_total=tot_sw;

			}else
				p_sw=psw;

			/*增加一条连接*/
			p_ps->switch_id=p_sw->id;
			p_ps->swithch_port=p_sw->countoflinks;
			sprintf(p_ps->switch_sid,"%s",p_sw->sid);
			p_ps->delay=1;//(float)(lower+(upper-lower)*random_021());
			p_ps->bandwidth=1024;
			

			/*更新交换机的连接信息*/
			p_sw->links[p_sw->countoflinks]=swlnk;
			p_sw->countoflinks++;
			
			tot_p2s++;/*p2s增加1*/
		}

		j++;/*读入下一个PM*/
	}

	
	_gthis->sd->p2s_total=tot_p2s;
	

	/*---生成交换机与交换机之间的连接--s2slist----*/
	tot_s2s=_gthis->sd->s2s_total;
	
	/*0层的每个交换机连接到1层的新交换机，连接关系记录在s2slist*/
	
	
	swnum0=tot_sw;/*430*/
	start_swid=0;

	swnum1=0;
	j=0;
	
	while(swnum0>1){
		
		level++;

		tot_sw=_gthis->sd->sw_total;

		for(i=0;i<swnum0;i++)/*0~429*/
		{
			/*取0层交换机*/
			
			p_sw=&_gthis->sd->swlist[start_swid+i];
		
			/*取1层交换机*/
			p_sw1=&_gthis->sd->swlist[tot_sw];
			if(i==0 && p_sw1->id>0)
			{
				swnum1++; tot_sw++;
				p_sw1=&_gthis->sd->swlist[tot_sw]; 
			}
			
			/*检查端口数有没有用完*/
			while((p_sw1->portnum>0) && (p_sw1->countoflinks==p_sw1->portnum))
			{
				swnum1++; tot_sw++;
				p_sw1=&_gthis->sd->swlist[tot_sw]; 
			}

			/*新的SW:新建一个交换机*/
			if(p_sw1->id==0){
				/*属性更新*/
				p_sw1->id=tot_sw;
				p_sw1->countoflinks=0;
				p_sw1->level=level;
				p_sw1->portnum=SW_PORT_NUM;
				sprintf(p_sw1->sid,"sw%d",tot_sw);
			}
		
			/*增加一条边*/
			p_ss=&_gthis->sd->s2slist[tot_s2s];
		
			/*更新属性*/
			p_ss->id=tot_s2s;
			p_ss->p_src_sw=p_sw; /*孩子交换机*/
			p_ss->sw_src_id=p_sw->id;
			sprintf(p_ss->sw_src_sid,"%s",p_sw->sid);

			p_ss->p_dst_sw=p_sw1;/*上一层父亲交换机*/
		
			p_ss->delay=1;//(float)(lower+(upper-lower)*random_021());
			p_ss->bandwidth=1024;
		
			/*更新连接信息*/
			swlnk.id=p_sw->id;
			swlnk.port=0;/*0端口上行口*/
			swlnk.type=SWITCHLINK;/*表示交换机*/
			p_sw1->links[p_sw1->countoflinks++]=swlnk;

			tot_s2s++;/*更新SS连接关系的总数*/
		}
		tot_sw++;swnum1++;
		
		/*更新交换机的总数*/
		//tot_sw+=swnum1;
		_gthis->sd->sw_total=tot_sw;
		/*更新SS连接关系的总数*/
		_gthis->sd->s2s_total=tot_s2s;
		start_swid += swnum0;
		swnum0=swnum1;/*17*/
		swnum1=0;
	}

	_gthis->sd->s2s_total=tot_s2s;
}



/*
* 函数名 : create_p2p(void)
* 功能 : 随机生成时延矩阵
* 传入参数 : 无
* 参数1 ： 
* 返回值 : 无
* @created by 孙业成 in 2013.5.13
*/
void create_p2p(double lower, double upper,int pmnum)
{
	st_pm2pm * t_p2plist =_gthis->sd->p2plist;//建立指针指向全局变量
	int src_id,dst_id;
	int t_total_pm;
	int i = 0;		//记录p2plist的ID
	t_total_pm= pmnum;
	set_seed ();
	for (src_id = 0; src_id < t_total_pm; src_id ++) {//对src_id从0到pm总数循环
		for(dst_id = src_id + 1; dst_id < t_total_pm; dst_id ++){	//对dst_id从0到pm总数循环
			(t_p2plist+i)->pm_src_id = src_id;
			sprintf((t_p2plist+i)->pm_src_sid, "pm%d", src_id);
			(t_p2plist+i)->pm_dst_id = dst_id;
			sprintf((t_p2plist+i)->pm_dst_sid, "pm%d", dst_id);
			(t_p2plist+i)->pm_dst_port = (int) random_a2b(0,5);		//随机选择网卡
			(t_p2plist+i)->pm_src_port = (int) random_a2b(0,5);
			(t_p2plist+i)->delay = random_a2b(0,7);				//随机生成时延
			(t_p2plist+i)->bandwidth = random_a2b(lower, upper);	//随机生成带宽
			i++;
			if(i >= P2P_TOTAL){
				LogMsg("overflow!\n");
				return;
			}
		}
	}
}

/*功能:统计热点机数和冷点机数*/
void count(void)
{
	int i,j,index;
	int hot,cold;
	printf("There are ");
	for(i=0,hot=0;i<_gthis->sd->pm_total;i++){//遍历所有物理机,有一个维度大于HOT的则为热点
		for(j=0;j<DIM;j++)
			if(_gthis->sd->pmlist[i].host.load[j].used / _gthis->sd->pmlist[i].host.load[j].total > HOT){
				hot++;
				break;
			}
	}
	printf("%d hot pm.\n",hot);//输出热点物理机台数

	printf("There are ");
	for(i=0,cold=0;i<_gthis->sd->pm_total;i++){//遍历所有物理机,其所有维度小于COLD的则为冷点
		for(j=0,index=1;j<DIM;j++)
			if(_gthis->sd->pmlist[i].host.load[j].used / _gthis->sd->pmlist[i].host.load[j].total > COLD){
				index = index&&0;
				break;
			}
			if(index&&_gthis->sd->pmlist[i].host.stat)
				cold++;
	}
	printf("%d cold pm.\n",cold);//输出冷点物理机台数

	printf("There are ");//输出孔物理机台数
	for(i=0,index=0;i<_gthis->sd->pm_total;i++){
		if(!_gthis->sd->pmlist[i].host.stat)
			index++;
	}
	printf("%d empty pm.\n",index);
}

/*将非空物理机的stat置为1,空物理机置为0*/
void set_stat(void)
{
	int i,j;
	int index;
	for(i=0;i<_gthis->sd->pm_total;i++){			//遍历所有物理机,并将空物理机的stat置为0
		for(j=0,index=1;j<DIM;j++)
			if(_gthis->sd->pmlist[i].host.load[j].used)
				index = index&&0;
		if(!index)
			_gthis->sd->pmlist[i].host.stat = 1;
	}
}