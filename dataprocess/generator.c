
/* �ļ�ע�� */
/*
* @file generate.c generate the simulation data
* @created by  sunyecheng <dangoakachan@foxmail.com> at 2013.05.11
* @modified by Jianhai Chen at 2013.05.13
*/

#include "generator.h"


/*�������4��PM*/
st_PMResVector PV={
	{400,800,1600,3200},/*100%�ٷֱ�*/
	{8192,16384,32768,65536}, /*MB*/
	{512,1024,2048,4096},/*GB*/
	{100,100,100,100},/*100%*/
	{1024,2048,3072,4096},/*mbps*/
};

st_pm PMV[4];
st_pm minPMV;


/*ģ�����ݣ�����PMV���4��*/
void createPMV()
{
	int i,j;
	
	/*ģ��VM��VM�ĸ���=ģ��VM*һ���������0.3*/
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
* ������ : void init_dataprocess(cls_dataprocessPtr THIS)
* ���� : ��ʼ�����ݴ�����ĳ�Ա����
* ������� : cls_dataprocessPtr THIS ���ݴ�����ָ��
* ����ֵ :��
* @created by Jianhai Chen in 2013.5.12
*/

void init_generator(cls_generatorPtr THIS)
{
	_gthis=THIS;   /*��ʼ���ڲ�thisָ��*/


	THIS->set_seed=set_seed;			//��ȡ���������
	THIS->create_frame=create_frame;
	THIS->create_group=create_group;

	THIS->random_021=random_021;		//�����������0-1�ڵ�ʵ��
	THIS->random_a2b=random_a2b ;	//�����������a-b�ڵ�ʵ��
	THIS->random_int_ab=random_int_ab;//�����������a��b
	THIS->ifiprepeat=ifiprepeat;		//���������ɵ�ip��ַ�Ƿ��ظ�
	THIS->ifv2vrepeat=ifv2vrepeat;		//���������ɵ�vm2vm�Ƿ��ظ�
	THIS->updatepm=updatepm;			//����һ���������,���������
	THIS->create_pm=create_pm;			//������������
	THIS->create_vm=create_vm;			//������������
	THIS->create_v2v=create_v2v;		//�������vm������ϵ
	THIS->create_v2v_no_aff=create_v2v_no_aff;		//�������vm�������׺͹�ϵ
	THIS->generate_data_by_vms=generate_data_by_vms;//�������vm�������׺͹�ϵ
	THIS->create_v2v_by_aff_type=create_v2v_by_aff_type;	//��������׺͹�ϵ
	THIS->create_v2v_by_ta=create_v2v_by_ta;
	THIS->clear_res=clear_res;			//��reservedȫ��Ϊ0
	THIS->count=count;					//ͳ���ȵ������������
	THIS->set_stat=set_stat;			//���ǿ��������stat��Ϊ1,���������Ϊ0
	THIS->createPMV=createPMV;
	THIS->addtov2vlist=addtov2vlist;
	THIS->create_empty_pm=create_empty_pm;
	THIS->set_empty_pm=set_empty_pm;	
	
	THIS->create_p2p_tree_topo=create_p2p_tree_topo;	
}



//
///***********************************************************************/
///*
// * ������ : void generator_fun(void)
// * ���� : ģ���������ɺ���,���ڼ��ɵ�����������ɺ���������ɺ���
// * ������� : ��
// * ����1 : 
// * ����ֵ : ��
// * @created by ��ҵ�� in 2013.5.11
// */
//void generator_fun(void)
//{
//	double lower,upper;									//����a��b���ڴ洢lower bound��upper bound
//	double total;
//	printf("please input total number of pm\n");
//	scanf("%d",&g_total_pm);								//�������������
//	printf("please input total number of vm\n");
//	scanf("%d",&g_total_vm);								//�������������
//	printf("please input �� and �� :(�� is lower bound, �� is upper bound)\n");
//	printf("����0,1�� = ");
//	scanf("%lf",&lower);										//���븺������
//	printf("�£�0,1�� = ");
//	scanf("%lf",&upper);										//���븺������
//
//	create_pm();
//	create_vm(lower,upper);
//
//	printf("please input traffic_rate lower-bound and upper-bound:\n");
//	printf("lower bound(0,10000) = ");
//	scanf("%lf",&lower);										//������������
//	printf("upper bound(0,10000) = ");
//	scanf("%lf",&upper);										//������������
//	printf("total border(0.2 or 0.3 or 0.4) = ");
//	scanf("%lf",&total);										//�������
//	create_v2v(lower,upper,total);
//}


/***********************************************************************/
/*
* ������ : void set_seed(void)
* ���� : ��ȡ���������
* ������� : ��
* ����1 : 
* ����ֵ : ��
* @created by ��ҵ�� in 2013.5.11
*/
void set_seed(void)	
{
	srand((unsigned) time(NULL));//��ȡϵͳʱ����Ϊ���������
}


/***********************************************************************/
/*
* ������ : double random(void)
* ���� : �����������[0,1)�ڵ�ʵ��
* ������� : ��
* ����1 : 
* ����ֵ : ���������ʵ��
* @created by ��ҵ�� in 2013.5.11
*/
double random_021(void)
{
	return rand()/(RAND_MAX+1.0);
}


/***********************************************************************/

/*
* ������ : double random_a2b(double a,double b)
* ���� : �����������[a,b]�ڵ�ʵ��
* ������� : double a, double b
* ����a : ��������
* ����b : ��������
* ����ֵ : ���������ʵ��
* @created by ��ҵ�� in 2013.5.11
*/
double random_a2b(double a,double b)//�����������[a,b]�ڵ�ʵ��
{
	return (b-a)*rand()/(RAND_MAX+0.0)+a;
}

/***********************************************************************/
/*
* ������ : int random_int_ab(int a,int b)
* ���� : �����������a��b
* ������� : int a, int b
* ����a : ����ı���
* ����b : ����ı���
* ����ֵ : ���������a��b
* @created by ��ҵ�� in 2013.5.11
*/
int random_int_ab(int a,int b)
{
	if(!(rand()%2))					//rand()Ϊż������a,���򷵻�b
		return a;
	else
		return b;
}

/***********************************************************************/
/*
* ������ : int ifiprepeat(int i,int j,int index)
* ���� : �ж�ip��ַ�Ƿ��ظ�����0--i-1̨��
* ������� : int i,int j,int index
* ����i : ��įpm��vm
* ����j : pm��vm�ĵ�j��ip(���ĸ�)
* ����index :index=0��ʾpm,1��ʾvm
* ����ֵ : return 1��ʾip��ַ���ظ�, return 0��ʾ���ظ�
* @created by ��ҵ�� in 2013.5.11
*/
int ifiprepeat(int i,int j,int index)
{
	int k,l;
	if(!index){	/*��������Ĳ����ж��Ǽ��vm����pm*/
		for(k=0;k<=i;k++)
			for(l=0;l<PORT_NUM;l++)
				/*�������pm��ip������pm��ip��ַ�Ա�*/
					if((!strcmp(_gthis->sd->pmlist[i].host.eip[j],
						_gthis->sd->pmlist[k].host.eip[l]))&&(k!=i&&l!=j))	
						return 0; //���
	}
	else{
		for(k=0;k<i;k++)
			if(!strcmp(_gthis->sd->vmlist[i].host.ip,_gthis->sd->vmlist[k].host.ip))					//�������vm��ip������vm��ip��ַ�Ա�
				return 0;//�ظ�
	}



	return 1;//���ظ�
}


/***********************************************************************/
/*
* ������ : int updatepm(int vmID,int pmID)
* ���� : ÿ����һ̨vm�����������pm��״̬
* ������� : int vmID,int pmID
* ����vmID : �������ID
* ����pmID : �������ID
* ����ֵ : return 1��ʾ���³ɹ�, return 0��ʾ�������������
* @created by ��ҵ�� in 2013.5.11
*/
int updatepm(int vmID,int pmID)
{
	double temp,resd,used;//��ʱ����,���ڴ洢������м�ֵ
	int k;

	/*Ԥ�ж��Ƿ�ŵ���PM*/
	for(k=0;k<DIM;k++){	//���ζ�8��ά�Ƚ��и���
		resd=_gthis->sd->pmlist[pmID].host.load[k].total - _gthis->sd->pmlist[pmID].host.load[k].reserved;
		used=_gthis->sd->vmlist[vmID].host.load[k].used + _gthis->sd->pmlist[pmID].host.load[k].used;
		temp =  resd- used;	//temp���ڸ��º��ʣ������
		if(temp<0)	//���tempС��0,��ʾ�������������,����0
			return 0;
	}

	/*��ɷ��ã�������PMʣ������*/
	for(k=0;k<DIM;k++)
		_gthis->sd->pmlist[pmID].host.load[k].used 
		+= _gthis->sd->vmlist[vmID].host.load[k].used;//���¸��������used

	return 1;
}


/***********************************************************************/
/*
* ������ : int createframe(int)
* ���� : ����������б�����PM
* ������� : numÿ�����PM����
* ����1 : 
* ����ֵ : �ɹ�-1��ʧ��-0�� _gthis->framelist�б�������Ϣ
* @created by �½��� in 2013.7.22
* @modify by ## in ####.##.##
*/
int create_frame(int num)
{
	int i,j;

	framePtr p_fr;
	pmPtr p_pm;

	/*�ܹ�����Ҫ�Ŀ�ĸ���100/18 = 5 10 */
	//fcc=_gthis->sd->pm_total/num+1;

	for(j=0;j<_gthis->sd->pm_total;j++)	
	{
		/*��ȡһ��PM*/
		p_pm=&_gthis->sd->pmlist[j];

		if(j%num==0) {
			i=j/num;
			p_fr=&_gthis->sd->framelist[i];
			p_fr->countofpm=0;
			strcpy(p_fr->pmids,p_pm->host.sid);/*����PMSID����*/
			p_fr->id=i;
			sprintf(p_fr->sid,"fr%d",i);
			_gthis->sd->fr_total++;
		}
		else{/*�����µ�PM*/
			sprintf(p_fr->pmids,"%s,%s",p_fr->pmids,p_pm->host.sid);
		}
		
		p_fr->i_pmids[j%num]=p_pm->host.id; /*����pm.id���ÿ�*/
			
		/*������1*/
		p_fr->countofpm++;
		
		/*����PM���ڿ�*/
		p_pm->at_p_fr=p_fr;/*�������ڿ�ָ��*/		
		strcpy(p_pm->at_fr_sid,p_fr->sid);
	}

	return  _gthis->sd->fr_total;
}



/***********************************************************************/
/*
* ������ : int create_group(int)
* ���� : �����ȡ����PM������һ�������PMLIST
* ������� : num, pm�ĸ���
* ����1 : 
* ����ֵ : �ɹ�-1��ʧ��-0�� _gthis->grouplist�б�������Ϣ
* @created by �½��� in 2013.7.22
* @modify by ## in ####.##.##
*/
int create_group(int start,int num)
{
	int i,j,kk,pid,flag,pmtot,grtot;
	pmgPtr p_gr;
	pmPtr p_pm;
	//_gthis->sd->gr_total=num;

	pmtot=_gthis->sd->pm_total;
	grtot=_gthis->sd->gr_total;/*��gr_total�Ļ����ϼ��������µ���*/

	/*ȡһ����*/	
	p_gr=&_gthis->sd->pmglist[grtot];		
	p_gr->id=grtot;
	sprintf(p_gr->sid,"gr%d",grtot);
	p_gr->countofpm=0;

	/*��ȡnum��PM*/
	for(i=0;i<num;i++){
		/*����û���ظ���PM*/
		flag=1;
		while(flag==1)
		{
			kk=start+i;//(int)(random_021()*pmtot); 
			p_pm=&_gthis->sd->pmlist[kk];
		
			/*����Ƿ��Ѿ�����*/
			flag=0;
			for(j=0;j<i;j++)
			{
				pid=p_gr->i_pmids[j];
				if(kk==pid) {
					flag=1;break;
				}			
			}
		}

		/*�ҵ���p_pm*/
		p_gr->i_pmids[i]=p_pm->host.id;
		p_gr->countofpm++;

		if(i==0)
			sprintf(p_gr->pmids,"%s",p_pm->host.sid);
		else
			sprintf(p_gr->pmids,"%s,%s",p_gr->pmids,p_pm->host.sid);		
		
		/*����PM��������Ϣ*/
		p_pm->at_p_pmgrps[p_pm->countofpmgrp]=p_gr;
		
		if(p_pm->countofpmgrp==0)
			strcpy(p_pm->at_gr_ids,p_gr->sid);
		else
			sprintf(p_pm->at_gr_ids,"%s,%s",p_pm->at_gr_ids,p_gr->sid);
		p_pm->countofpmgrp++;

	}

	/*�鴴����ϣ���������1*/
	_gthis->sd->gr_total++;

	return 1;
}

/***********************************************************************/
/*
* ������ : void createIP(char *ip12, char *retip)
* ���� : ���һ��IP��ַ
* ������� : (char *ip12, char *retip)
* ����ip12 : IP��ַ��ǰ�����������ַ�������(10.10....)
* ����retip: ָ�����ɺ󷵻ص�IP: ����4���Σ�10.10.2.234��
* ����ֵ : ��
* @created by �½��� in 2013.7.13
*/

void createIP(char *ip12, char *retip)
{
	int t1,t2;
	char rip[20];
	t1=(int)random_a2b(1,255);//���ɵ�3������
	t2=(int)random_a2b(1,255); //���ɵ�4������
	sprintf(retip,"%s.%d.%d",ip12,t1,t2);
}




/***********************************************************************/
/*
* ������ : int createpm(int)
* ���� : �������������б�
* ������� : pm_num, �������̨��
* ����1 : 
* ����ֵ : �ɹ�-1��ʧ��-0�� _gthis->pmlist�б�������Ϣ
* @created by ��ҵ�� in 2013.5.11
* @modify by sun in 2013.6.17
*/
int create_pm(int num,int hotpm,int coldpm)
{
	int i,j,d;	//i,j��Ϊѭ��ʽʹ�ñ���
	int temp1,temp2;	//temp1,temp2���ڴ��ip��ַ�ĺ�����
	double reserved; //���ڴ���������reserved�������
	char eth[10];/*������������*/

	set_seed();		//��ȡ���������
	for(i=0;i<num;i++){//ÿ����һ̨pmѭ��һ��

		_gthis->sd->pmlist[i].host.id = i;//id��˳������*/
		sprintf(_gthis->sd->pmlist[i].host.sid,"pm%d",i);
		
		for(j=0;j<PORT_NUM;j++){
			/*����IP��ַ*/
			temp1=(int)random_a2b(1,255);//temp����1��255�������(����1,��������255)
			temp2=(int)random_a2b(1,255);
			sprintf(_gthis->sd->pmlist[i].host.eip[j],"10.10.%d.%d",temp1,temp2);	//��ʽ��ip��ַ

			/*���ip��ַ�Ƿ����ظ�,���ظ�����������*/
			//if(!ifiprepeat(i,j,0))	j--;
		}

		_gthis->sd->pmlist[i].host.stat=0;
		strcpy(_gthis->sd->pmlist[i].host.ip,_gthis->sd->pmlist[i].host.eip[0]);	//ȷ��һ����ip��ַ

		/*����cpu,�칹:����Ϊ4,8,16,32c*/
		strcpy(_gthis->sd->pmlist[i].host.load[0].metric_name,"cpu");
		_gthis->sd->pmlist[i].host.load[0].used=0;
		_gthis->sd->pmlist[i].host.load[0].total=random_int_ab(random_int_ab(4,8),random_int_ab(16,32));

		/*����mem,�칹:���涨Ϊ4G,8G,16G,32G,64G*/
		strcpy(_gthis->sd->pmlist[i].host.load[1].metric_name,"mem");
		_gthis->sd->pmlist[i].host.load[1].used=0;
		_gthis->sd->pmlist[i].host.load[1].total
			=random_int_ab(random_int_ab(8000,16000),random_int_ab(4000,32000));	

		/*����һ��������������Դ����:��λ��bps�������1GB,2GB,3GB,4GB*/
		for(d=0;d<PORT_NUM;d++){
			sprintf(eth,"eth%d",d);
			strcpy(_gthis->sd->pmlist[i].host.load[d].metric_name,eth);
			_gthis->sd->pmlist[i].host.load[d].used=0;
			_gthis->sd->pmlist[i].host.load[d].total=100;		
		}
		
		/*����2��������������Դ����*/
		/*strcpy(_gthis->sd->pmlist[i].host.load[3].metric_name,"eth1");
		_gthis->sd->pmlist[i].host.load[3].used=0;
		_gthis->sd->pmlist[i].host.load[3].total=100;
		*/
		/*����3����������������Դ����*/
		/*strcpy(_gthis->sd->pmlist[i].host.load[4].metric_name,"eth2");
		_gthis->sd->pmlist[i].host.load[4].used=0;
		_gthis->sd->pmlist[i].host.load[4].total=100;
		*/
		/*����4����������������Դ����*/
		/*strcpy(_gthis->sd->pmlist[i].host.load[5].metric_name,"eth3");
		_gthis->sd->pmlist[i].host.load[5].used=0;
		_gthis->sd->pmlist[i].host.load[5].total=100;
		*/
		
		/*���ɴ���IO������������GB*/
		strcpy(_gthis->sd->pmlist[i].host.load[6].metric_name,"diskio");
		_gthis->sd->pmlist[i].host.load[6].used=0;
		_gthis->sd->pmlist[i].host.load[6].total=100;

		
		/*����FPGA*/
		strcpy(_gthis->sd->pmlist[i].host.load[7].metric_name,"fpga");
		_gthis->sd->pmlist[i].host.load[7].used=0;
		_gthis->sd->pmlist[i].host.load[7].total=100;

		if(i < hotpm)
			reserved = 0;		//������С��ʹ�������Ϊ�ȵ�
		else if(i < coldpm+hotpm)
			reserved = random_a2b(HOT,1);//��������ʹ�������Ϊ���
		else
			reserved = random_a2b(COLD,HOT);

		for(j=0;j<DIM;j++){//���ñ������Ա��������������
			_gthis->sd->pmlist[i].host.load[j].reserved 
				= _gthis->sd->pmlist[i].host.load[j].total * reserved;//???
		}
	}

	return 1;
}
/*hcv:��ʾһ���������ȳ̶ȵ�ϵ����0.8��ʾһ���޶�����Գ���hcv������0.8*/
/*�ж�vm�Ƿ��ܷŵ���pm,�ŵ��·���1���Ų��·���0*/
int canPack(vmPtr p_vm, pmPtr p_pm, double hcv)
{
	int i,flag;
	double s,t;
	flag=1;
	for(i=0;i<DIM;i++)
	{
		s=p_pm->host.load[i].used+p_pm->host.load[i].reserved;
		s = s + p_vm->host.load[i].total;/*����total��ʾ����*/
		t = p_pm->host.load[i].total;
		if(s > (t*hcv)){
			flag=0;break;
		}		
	}

	return flag;
}

/*����vm��pm*/
void update_vm_to_pm(vmPtr p_vm, pmPtr p_pm)
{
	int i;
	double s;
	
	for(i=0;i<DIM;i++)
	{
		s=p_pm->host.load[i].used;
		s+=p_vm->host.load[i].total;/*����total��ʾ����*/
		p_pm->host.load[i].used=s;
	}

	p_vm->at=p_vm->at_pmid=p_pm->host.id;
	
}



/**********************************************************************/
/*�������ɹ���
����VM����ѭ��
ÿ����һ��VM������һ��PM����VMװ�룬
����һ��PM��һ��VM��
������vmnum,VM����  a,b����[0,1),��ʾ���صķ�Χ
     int hotnum  >0.8,int coldnum <0.2
�������½���
*/

void generate_data_by_vms(int vmnum, float a,float b,float hv)
{
	int i,j,k,ip3,ip4,flag;
	//int cp=1;/*1-��ʾҪ����PM�� 0-��ʾ���ô���*/
	double rr,rp;	
	int tot_pm,tot_vm;
	vmPtr pv;pmPtr pp;st_vm vm;st_pm pm;
		
	tot_pm=_gthis->sd->pm_total;
	tot_vm=_gthis->sd->vm_total;

	
	i=0;j=0;
	/*��PV[0]*/
	while(i<vmnum){
		
		/*����һ̨VM*/
		vm.host.id=tot_vm+i;
		sprintf(vm.host.sid,"vm%d",tot_vm+i);
		vm.host.stat=0;
		/*����IP*/
		ip3=(int)(random_021()*255);
		ip4=(int)(random_021()*255);
		sprintf(vm.host.ip,"10.10.%d.%d",ip3,ip4);

		/*8��ά���������ɣ���PMVģ���������*/
		for(k=0;k<DIM;k++){
			rr=a+(b-a)*random_021();/*�����0-1,��a,b֮��*/
			//strcpy(vm.host.load[k].metric_name, sh.getName(k));
			vm.host.load[k].total=minPMV.host.load[k].total*rr;
			vm.host.load[k].used=vm.host.load[k].total;
			vm.host.load[k].reserved=0;
		}

		/*����VM��vmlist*/
		_gthis->sd->vmlist[tot_vm+i]=vm;
		
		pv=&_gthis->sd->vmlist[tot_vm+i];/*ָ���1VM*/
		
		/*************************************************/
		/*Ѱ��PM����*/
		while(1){
			/*�ж�vm�Ƿ�ŵ���pm��	���Ѿ�������PM��
			���ҵ�һ�����Էŵ��µ�PM������*/
			flag=0;
			for(k=tot_pm;k<tot_pm+j;k++){
				pp=&_gthis->sd->pmlist[k];
				if(canPack(pv,pp,hv)){
					flag=1;
					break;
				}
			}

			if(flag==1)/*��ʾ�Ѿ��ҵ����Է����PM*/
			{
				/*����vm������pm��used*/
				update_vm_to_pm(pv,pp);

				/*����VM���ڵ�PM*/
				pv->at_pmid=pp->host.id;
				pv->at=pv->at_pmid;/*Ĭ��at��PMID*/
				
				/*����PM��������VM����*/
				pp->p_vms[pp->countofvm]=pv;/*����*/
				pp->countofvm++;/*vm��������1*/
				
				break;/*�˳�����PM*/
			}
			else /*û���ҵ����¿�PM*/
			{
				
				rp=4*random_021();/*������4�������ѡ��һ����Ϊģ�帴��������PM*/
				
				/*����һ��PM����VM����*/
				pm=PMV[(int)rp];
				
				pm.host.id=tot_pm+j;
				pm.host.stat=0;
				sprintf(pm.host.sid, "pm%d",tot_pm+j);

				/*����IP*/
				ip3=(int)(random_021()*255);
				ip4=(int)(random_021()*255);
				sprintf(pm.host.ip,"10.10.%d.%d",ip3,ip4);
				pm.countofvm=0;
				
				/*��PM����PMLIST*/
				_gthis->sd->pmlist[tot_pm+j]=pm;
				
				j++;/*PM�±�����1��׼��������һ��PM*/
				_gthis->sd->pm_total++;
			}
		}
		
		/*׼����һ��VM*/
		i++;
		/*�ۼ�VM����*/
		_gthis->sd->vm_total++;
	}
}

/***********************************************************************/
/*
* ������ : void create_empty_pm(int pmnum)
* ���� : �������������б�
* ������� : int pmnum
* ���� pmnum : ��Ҫ���ɵ�PM����
* ����ֵ : ��
* @created by �½��� in 2013.7.26
* @modify by  in 2013.7.26
*/
void create_empty_pm(int pmnum)
{
	int i,rp,ip3,ip4;
	int tot_pm;
	st_pm pm;

	tot_pm=_gthis->sd->pm_total;
	
	createPMV();/*����ģ��*/

	for(i=0;i<pmnum;i++){
		rp=(int)4*random_021();/*������4�������ѡ��һ����Ϊģ�帴��������PM*/
				
		/*����һ��PM����VM����*/
		pm=PMV[(int)rp];
				
		pm.host.id=tot_pm+i;
		pm.host.stat=0; /*����PM*/
		sprintf(pm.host.sid, "pm%d",tot_pm+i);

		/*����IP*/
		ip3=(int)(random_021()*255);
		ip4=(int)(random_021()*255);
		sprintf(pm.host.ip,"10.10.%d.%d",ip3,ip4);
		pm.countofvm=0;
				
		/*��PM����PMLIST*/
		_gthis->sd->pmlist[tot_pm+i]=pm;
		
		_gthis->sd->pm_total++;
	}
}



/***********************************************************************/
/*
* ������ : void set_empty_pm()
* ���� : ���PMΪ����
* ������� : pmPtr pmlist ��ʾҪ�ÿ��ص�PM����
* ����ֵ : ��
* @created by �½��� in 2013.7.26
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

		ppm->host.stat=0;/*0-������״̬*/

		ppm->countofvm=0;
	}
}



/***********************************************************************/
/*
* ������ : void createvm(double a,double b��int vmnum,int pmnum, int hotpm,int coldpm)
* ���� : �������������б�
* ������� : double a, double b��int vmnum,int pmnum, int hotpm,int coldpm
* ����a,b : ָ��ֵ�ķ�Χ�߽�ֵ����cpu�����ʵķ�Χ��0��1 �� 0~32����
* ����vmnum, pmnum : ��Ҫ���ɵ�VM\PM����
* ����int hotpm,int coldpm�� �ȵ�PM���������PM�������º�WARM��������PM����pmnum-hot-cold
* ����ֵ : ��
* @created by ��ҵ�� in 2013.5.11
* @modify by sun in 2013.6.17
*/
void create_vm(double a,double b,int vmnum,int pmnum, int hotpm,int coldpm)
{
	int AT,k;
	int i;

	set_seed();		//��ȡ���������

	for(i=0;i<vmnum;i++){
		
		_gthis->sd->vmlist[i].host.id = i;//��������ID
		sprintf(_gthis->sd->vmlist[i].host.sid,"vm%d",i); //��������ID

		//����IP��ַ
		createIP(DEFIP,_gthis->sd->vmlist[i].host.ip);

		//�ж�ip��ַ�Ƿ��Ѿ����ڻ��ظ�,���ظ�����������
		if(!ifiprepeat(i,0,1)){
			i--;
			continue;
		}
	
		if(i<hotpm+coldpm)
			AT = i; //��֤ǰhotpm̨�ȵ㼰��ǰcoldpm̨����û�п������
		else
			AT=_gthis->sd->vmlist[i].at = (int) (random_021()* pmnum);//����������������

		for(k=0;k<DIM;k++){
			/*���ν�8��ά�ȵ����ָ�ֵ*/
			strcpy(_gthis->sd->vmlist[i].host.load[k].metric_name,
				_gthis->sd->pmlist[AT].host.load[k].metric_name);	
		
			switch(k){
				case ETH0:case ETH1:case  ETH2: case ETH3:
					
					/*�����ÿһ���������ɸ���,�����Ϊ0*/
					_gthis->sd->vmlist[i].host.load[k].used 
						= _gthis->sd->vmlist[i].host.load[k].total 
						= (_gthis->sd->pmlist[AT].host.load[k].total 
						- _gthis->sd->pmlist[AT].host.load[k].reserved) * random_a2b(a,b);
					//_gthis->sd->vmlist[i].host.link_port = k - 2;//�洢���ɵ��������
			
					break;
				case CPU: case MEM: case FPGA: case DISKIO:

					_gthis->sd->vmlist[i].host.load[k].used
					= _gthis->sd->vmlist[i].host.load[k].total
					= (_gthis->sd->pmlist[AT].host.load[k].total 
						- _gthis->sd->pmlist[AT].host.load[k].reserved) * random_a2b(a,b);
					break;
			}
	
		}

//		k=(int)random_a2b(2,6);//��ʵ���������2-5

		
		if(!updatepm(i,AT)){
			//�������������,���ж��Ƿ�������,�����������򻻵ڶ��ַ�ʽ����
			//��һ�ַ�ʽ����,vm�ĸ���=(pm.total-pm.reserved(������))*�����
			//�ڶ��ַ�ʽ����,vm�ĸ���=(pm��ʣ��ռ�-������)*�����
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
/*�������������reserved��Ϊ0*/
void clear_res(void)
{
	int i,j;
	for(i=0;i<_gthis->sd->pm_total;i++)						//�������������
		for(j=0;j<DIM;j++)
			_gthis->sd->pmlist[i].host.load[j].reserved = 0;
}


/***********************************************************************/
/*
* ������ : ifv2vrepeat(int src_id,int dst_id)
* ���� : ���������ɵ�vm2vm�Ƿ��ظ�
* ������� : int i,int j, int ID
* ����src_id : src_id��ԴvmID
* ����dst_id : dst_id��Ŀ��vmID
* ����ID : _gthis->sd->v2vlist
* ����ֵ : return 1��ʾ���ظ�, return 0��ʾ���ظ�
* @created by �½��� in 2013.7.25
*/
int if_v2v_exist(int src_id,int dst_id,int afftyp)
{
	int i,flag;
	vm2vmPtr p_vv;

	if(afftyp <=0){
		/*��֮ǰ���ɵ�������ϵ���αȽ�,�����ظ���return 0*/
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
	else /*ͬ��*/
	{
		/*ͬ��ͬ��ͬ��*/
		/*ͬ������Ѿ����ù�ͬ������Ҫ�����ã�
		��"vm1,vm2,vm3,vm4",����һ��"vm1,vm2"��
		�����ԣ�"vm2,vm3"���������ԣ�������"vm1,vm2"�����׺ͣ������׺ͣ�
		�ǿ��Ե�
		ͬ�壺�����׺ͣ�������ͬ�壬��ͬ�򣬲�ͬ�飻
		*/
		/*��֮ǰ���ɵ�������ϵ���αȽ�,�����ظ���return 0*/
		
		
		
		for (i=0; i < _gthis->sd->v2v_total-1; i++)
		{
			p_vv=&_gthis->sd->v2vlist[i];
			if(p_vv->affinity ==afftyp)
			{/*�Ե�ǰ�������׺Ͷԣ�����Ƿ��Ѿ�����*/
				
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
 *����һ��VM��v2vlist:ͬ�塢ͬ��ͬ��
 *pv��ָ�룬ָ��һ�����飬num�Ǹ���
 *�����ߣ��½�����2013-07-26
 */
void addtov2vlistbyvms(vmPtr *pv,int num,int afftype)
{
	int i,tot_vv;
	char vmids[200];
	vm2vmPtr pvv;
	vmPtr pvm;
	tot_vv=_gthis->sd->v2v_total;

	/*��v2vlistĩβ������һ��V2V�±���tot_vv*/
	pvv=&_gthis->sd->v2vlist[tot_vv];
	pvv->id=tot_vv;
	pvv->countofvm=num;
	pvv->affinity=afftype;
	
	/*����vmids*/
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

	/*����V2Vlist*/
	tot_vv++;
	_gthis->sd->v2v_total=tot_vv;	
}

/********************************************************
��ͬ�鷴�׺�:-3���ڲ�ͬ����������VM����Ϊ��ͬ���׺�
���ȡ������ͬ�飬����ÿ�������ѡȡһ��PM��Ȼ��ֱ�
ѡ��������ͬPM�е�VM���������׺͹�ϵ
fff: 1-������ɣ� 0-ָ������
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
		/*1�����ѡ��������ͬ����*/
		tot_gr=_gthis->sd->gr_total;
		f1=f2=0;
		while(f1==f2){
			f1=(int)(tot_gr*random_021()); 		
			f2=(int)(tot_gr*random_021()); 		
		}
	
		p_gr1=&_gthis->sd->pmglist[f1];
		p_gr2=&_gthis->sd->pmglist[f2];
		/*---------------------------------------------*/

		/*2.��ÿ�������ѡ��PM*/
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
	
		/*3.��ÿ��PM�����ѡ��һ��VM*/
		flag=0;
		while(flag==0){
			vmc1=p_pm1->countofvm;
			r1=(int)(vmc1*random_021());
			p_vm1=p_pm1->p_vms[r1];
		
			vmc2=p_pm2->countofvm;
			r2=(int)(vmc2*random_021());
			p_vm2=p_pm2->p_vms[r2];
	
			/*�Ƿ����*/
			if(conflict(p_vm1->host.id,p_vm2->host.id,-3)==1)	{ 
				flag=0;/*��������Ҫ��������*/
			}
			else flag=1;
		}
	}
	if(fff==0)
	{
		/*ѡȡ��1�����,ѡȡ1��PM��ѡȡһ��VM*/
		tot_gr=_gthis->sd->gr_total;
gg1:
		printf("�����1�����0-%d",tot_gr-1);
		scanf("%d",&f1);
		p_gr1=&_gthis->sd->pmglist[f1];
		if(p_gr1->countofpm==0) goto gg1;
		
gg2:
		printf("�����2�����0-%d",tot_gr-1);
		scanf("%d",&f2);
		if(f1==f2) {printf("��2��������1����Ų�����ͬ�����������룺"); goto gg2;}
		p_gr2=&_gthis->sd->pmglist[f2];
		if(p_gr2->countofpm==0) goto gg2;
		/*2.��ÿ��������Ҫѡ��PM��*/

ppp3:
		tot_pm=p_gr1->countofpm;
		
		printf("������1�ĵ�1��PM�ţ�\n PM��[VM����]\n");
		for(i=0;i<tot_pm;i++)
		{
			p1=p_gr1->i_pmids[i];
			p_pm1=&_gthis->sd->pmlist[p1];
			
			if(i==0) printf("%d[%d]",p1,p_pm1->countofvm);
			else printf(",%d[%d]",p1,p_pm1->countofvm);
		}
		printf("\n");
		/*�����һ��PM����VM���*/
		scanf("%d",&p1);
		p_pm1=&_gthis->sd->pmlist[p1];
		/*��ʾ��PM�µ�����VMID��ѡȡһ��VM*/
		for(i=0;i<p_pm1->countofvm;i++)
		{
			if(i==0) printf("%d[%s]",i,p_pm1->p_vms[i]->host.sid);
			else printf(",%d[%s]",i,p_pm1->p_vms[i]->host.sid);
		}
		printf("\n������ѡVM����ţ�0-%d",p_pm1->countofvm);
		scanf("%d",&r1);
		p_vm1=p_pm1->p_vms[r1];
		
ppp4:
		tot_pm=p_gr2->countofpm;
		
		printf("�����2��PM��������2��\n PM��[VM����]");
		for(i=0;i<tot_pm;i++)
		{
			p2=p_gr2->i_pmids[i];
			p_pm2=&_gthis->sd->pmlist[p2];
			
			if(i==0) printf("%d[%d]",p2,p_pm2->countofvm);
			else printf(",%d[%d]",p2,p_pm2->countofvm);
		}
		/*����ڶ���PM����VM���*/
		scanf("%d",&p2);
		p_pm2=&_gthis->sd->pmlist[p2];
		/*��ʾ��PM�µ�����VMID��ѡȡһ��VM*/
		for(i=0;i<p_pm2->countofvm;i++)
		{
			if(i==0) printf("%d[%s]",i,p_pm2->p_vms[i]->host.sid);
			else printf(",%d[%s]",i,p_pm2->p_vms[i]->host.sid);
		}
		printf("\n������ѡVM����ţ�");
		scanf("%d",&r2);
		p_vm2=p_pm2->p_vms[r2];
				
	}
	/*��ȡ����VM��p_vm1,p_vm2*/
	/*����һ���׺ͱߣ����뵽v2vlist*/
	/*���뵽v2vlistĩβ*/
	addtov2vlist(p_vm1,p_vm2,0,0,-3);
}

/********************************************************
��ͬ���׺�:-2���ڲ�ͬ����������VM����Ϊ��ͬ���׺�
���ȡ������ͬ������ÿ�������ѡȡһ��PM��Ȼ��ֱ�
ѡ��������ͬ��VM���������׺͹�ϵ
--------------------------------------------------------*/
void createOnev2v_2()
{
	int vmc1,vmc2,tot_pm,tot_fr;
	int r1,r2,p1,p2,f1,f2,flag;
	pmPtr p_pm1,p_pm2;
	vmPtr p_vm1,p_vm2;
	framePtr p_fr1,p_fr2;
	
	/*1�����ѡ��������ͬ�Ŀ�*/
	tot_fr=_gthis->sd->fr_total;
	f1=f2=0;
	while(f1==f2){
		f1=(int)(tot_fr*random_021()); 		
		f2=(int)(tot_fr*random_021()); 		
	}
					
	p_fr1=&_gthis->sd->framelist[f1];
	p_fr2=&_gthis->sd->framelist[f2];
	/*---------------------------------------------*/

	/*2.��ÿ�������ѡ��PM*/
	tot_pm=p_fr1->countofpm;
	p1=p_fr1->i_pmids[(int)(tot_pm*random_021())];

	tot_pm=p_fr2->countofpm;
	p2=p_fr2->i_pmids[(int)(tot_pm*random_021())];

	p_pm1=&_gthis->sd->pmlist[p1];
	p_pm2=&_gthis->sd->pmlist[p2];

	/*3.��ÿ��PM�����ѡ��һ��VM*/
	flag=0;
	while(flag==0){
		vmc1=p_pm1->countofvm;
		r1=(int)(vmc1*random_021());
		p_vm1=p_pm1->p_vms[r1];
	
		vmc2=p_pm2->countofvm;
		r2=(int)(vmc2*random_021());
		p_vm2=p_pm2->p_vms[r2];
					
		/*�Ƿ����*/
		if(conflict(p_vm1->host.id,p_vm2->host.id,-2)==1)	{ 
			flag=0;/*��������Ҫ��������*/
		}
		else flag=1;
	}

	/*��ȡ����VM��p_vm1,p_vm2*/
	/*����һ���׺ͱߣ����뵽v2vlist*/
	/*���뵽v2vlistĩβ*/
	addtov2vlist(p_vm1,p_vm2,0,0,-2);

}



/********************************************************
��ͬ�巴�׺�:-1���ڲ�ͬһ��PM��������VM����Ϊͬ��������׺�
      ���ѡȡ����PM���ֱ�ѡ��������ͬ��VM���������׺͹�ϵ
--------------------------------------------------------*/
void createOnev2v_1(int afftyp)
{
	int vmc1,vmc2,tot_pm;
	int r,r0,r1,r2,flag;
	pmPtr p_pm1,p_pm2;
	vmPtr p_vm1,p_vm2;
	
	/*����:��������ͬPM��������VM����Ϊͬ�巴�׺�*/
	
	/*�ҵ�һ��*/
	vmc1=0;
	while(vmc1<1){ 
		/*�����һ��PM*/
		tot_pm=_gthis->sd->pm_total;
		r=(int)(tot_pm*random_021());
		p_pm1=&_gthis->sd->pmlist[r];

		/*��ȡ��PM�е�VMS����*/
	   	/*�������������������һ��PM����*/
		vmc1=p_pm1->countofvm;
	}
	
	/*�ҵ�һ��*/
	vmc2=0;
	r0=r;
	while(vmc2<1 || r0==r){ 
		/*�����һ��PM*/
		tot_pm=_gthis->sd->pm_total;
		r0=(int)(tot_pm*random_021());
		p_pm2=&_gthis->sd->pmlist[r0];
		/*��ȡ��PM�е�VMS����*/
	   	/*�����������1������һ��PM����*/
		vmc2=p_pm2->countofvm;
	}
	

	/*׼�����������*/	
	r1=r2=0;				
	/*VM��>=2*/
	flag=0;
	while(flag==0){				
		r1=(int)(vmc1*random_021());
		r2=(int)(vmc2*random_021());
					
		/*��ȡ����VM*/
		p_vm1=p_pm1->p_vms[r1];
		p_vm2=p_pm2->p_vms[r2];;
					
		/*�Ƿ����*/
		if(conflict(p_vm1->host.id,p_vm2->host.id,-1)==1)	{ 
			flag=0;/*��������Ҫ��������*/
		}
		else
			flag=1;
	}


	/*��ȡ����VM��p_vm1,p_vm2*/
	/*����һ���׺ͱߣ����뵽v2vlist*/
	/*���뵽v2vlistĩβ*/
	addtov2vlist(p_vm1,p_vm2,0,0,-1);

}




/*����:��ͬһ��PM��������VM����Ϊ�����׺�*/
void createOnev2v0(float a,float b)
{
	int vmcount,tot_pm;
	int r,r1,r2,flag;
	pmPtr p_pm;
	vmPtr p_vm1,p_vm2;

	/*����:��ͬһ��PM��������VM����Ϊͬ��������׺�*/
	vmcount=0;

	while(vmcount<4){ 
		/*�����һ��PM*/
		tot_pm=_gthis->sd->pm_total;
		r=(int)(tot_pm*random_021());
		p_pm=&_gthis->sd->pmlist[r];

		/*��ȡ��PM�е�VMS����*/
	   	/*�������������������һ��PM����*/
		vmcount=p_pm->countofvm;
	}
	
	/*׼�����������*/	
	r1=r2=0;				
	/*VM��>=2*/
	flag=0;
	while(flag==0||r1==r2){				
		r1=(int)(vmcount*random_021());
		r2=(int)(vmcount*random_021());
					
		/*��ȡ����VM*/
		p_vm1=p_pm->p_vms[r1];
		p_vm2=p_pm->p_vms[r2];;
					
		/*�Ƿ����*/
		if(conflict(p_vm1->host.id,p_vm2->host.id,0)==1)	{ 
			flag=0;/*��������Ҫ��������*/
		}
		else
			flag=1;
	}


	/*��ȡ����VM��p_vm1,p_vm2*/
	/*����һ���׺ͱߣ����뵽v2vlist*/
	/*���뵽v2vlistĩβ*/
	addtov2vlist(p_vm1,p_vm2,a,b,0);

}


/********************************************************
ͬ���׺�:1����ͬһ��PM�������ɸ�VM����Ϊͬ���׺�
      ����������������ѡȡPM���ֱ�ѡ��VM������ͬ���׺͹�ϵ
--------------------------------------------------------*/
void createOnev2v1()
{
	int vmcount,vmcc,tot_pm;
	int r,r1,flag,i,j;

	pmPtr p_pm,pmlst[4000];
	vmPtr pvm,p_vm[20];/*ͬ����಻����20��*/


	/*ͬ��,�������ͬ��VM�ĸ���>=2*/
	tot_pm=_gthis->sd->pm_total;
	/*�ҳ�VM��������2��PM*/
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
		/*��ȡ��PM�е�VMS*/
		vmcount=p_pm->countofvm-p_pm->cost;
	}

	vmcc=0;/*����*/
	while(vmcc<2||vmcc>=20){
		vmcc=(int)(vmcount*random_021());					
	}
	
	if(vmcc>=vmcount) vmcc=vmcount;

	/*���vmcc��ͬ���VM*/
	j=0;
	while(j<vmcc)
	{
		flag=0;
		while(flag==0)
		{
			/*���һ��VM*/
			r1=(int)(vmcount*random_021());/*�����ȡһ��*/
			pvm=p_pm->p_vms[r1];
			/*�ж�pvm�Ƿ��Ѿ�����*/
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

	

	/*����v2vlist*/
	addtov2vlistbyvms(p_vm,vmcc,1);
}


/********************************************************
ͬ���׺�:1����ͬһ�����������ɸ�PM����������VM����Ϊͬ���׺�
      ����������������ѡȡPM���ֱ�ѡ��VM������ͬ���׺͹�ϵ
--------------------------------------------------------*/
void createOnev2v2()
{
	int j,k,flag,tot_pm,tot_fr,vmcc;
	int r;
	framePtr p_fr1;
	vmPtr p_vm,p_vms[20]; /*ͬ���VM����������50*/
	pmPtr p_pm;
	
	/*��ȡһ����*/
	tot_fr=_gthis->sd->fr_total;
	p_fr1=&_gthis->sd->framelist[(int)(tot_fr*random_021())];

	/*ѡ��PM*/
	tot_pm=p_fr1->countofpm;
		
	/*����Ҫ��ͬ���VM����*/
	vmcc=0;
	while(vmcc<2||vmcc>=20)/*����2��*/
	{
		vmcc=(int)(20*random_021());					
	}


	j=0;k=0;
	while(j<vmcc||flag==1)
	{
		/*���һ��PM*/
		r=p_fr1->i_pmids[(int)(tot_pm*random_021())];
		p_pm=&_gthis->sd->pmlist[r];

		/*������һ��vm*/
		p_vm=p_pm->p_vms[(int)(p_pm->countofvm*random_021())];

		/*�Ƿ��Ѿ�����*/
		flag=0;
		for(k=0;k<j;k++){
			if(p_vm->host.id==p_vms[k]->host.id){
				flag=1;break;
			}
		}

		if(flag==0)
			p_vms[j++]=p_vm;

		/*ѭ����20000���ϻ�û�ҵ�����ǿ���˳�*/
		if(k>20000) break; 
		k++;
	}

	/*���뵽v2vlist*/
	if(k<20000)
		addtov2vlistbyvms(p_vms,vmcc,2);

}


/********************************************************
ͬ���׺�:1����ͬһ�����������ɸ�PM����������VM����Ϊͬ���׺�
      ����������������ѡȡPM���ֱ�ѡ��VM������ͬ���׺͹�ϵ
--------------------------------------------------------*/
int createOnev2v3()
{
	int j,k,flag,tot_pm,tot_gr,vmcc;
	int r,r1,r2;
	pmgPtr p_gr1;
	vmPtr p_vm,p_vms[20]; /*ͬ���VM����������50*/
	pmPtr p_pm;

	/*��ȡһ����*/
	tot_gr=_gthis->sd->gr_total;
	p_gr1=&_gthis->sd->pmglist[(int)(tot_gr*random_021())];

	
	/*ѡ��PM*/
	tot_pm=p_gr1->countofpm;
		
	/*����Ҫ��ͬ���VM����*/
	vmcc=0;
	while(vmcc<2||vmcc>=20)/*����2��*/
	{
		vmcc=(int)(20*random_021());					
	}

	j=0; k=0;
	while(j<vmcc||flag==1)
	{
		/*���һ��PM*/
		r1=(int)(tot_pm*random_021());
		r=p_gr1->i_pmids[r1];
		p_pm=&_gthis->sd->pmlist[r];

		/*������һ��vm*/
		r2=(int)(p_pm->countofvm*random_021());
		p_vm=p_pm->p_vms[r2];

		/*�Ƿ��Ѿ�����*/
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


	/*���뵽v2vlist*/
	if(k<20000){
		addtov2vlistbyvms(p_vms,vmcc,3);
		return 1;
	}
	else
		return 0;
}

/*���뵽v2vlist*/
void addtov2vlist(vmPtr p_vm1,vmPtr p_vm2,float a,float b,int aftype)
{
	int tot_v2v;
	vm2vmPtr p_vv;

	tot_v2v=_gthis->sd->v2v_total;
	p_vv=&_gthis->sd->v2vlist[tot_v2v];
	
	/*����v2v*/
	p_vv->id=tot_v2v;
	p_vv->traffic=(aftype==0)?a+(b-a)*random_021():0; 	
	p_vv->affinity=aftype;
	p_vv->delay=0;
	
	if(aftype<=0)/*�������׺���������VM�ǳɶԵģ�ֱ�Ӹ���v2v*/
	{		
		/*����*/
		p_vv->p_src_vm=p_vm1;
		p_vv->p_dst_vm=p_vm2;
		p_vv->countofvm=2;
		p_vv->p_vms[0]=p_vm1;
		p_vv->p_vms[1]=p_vm2;
		sprintf(p_vv->vmids,"%s,%s",p_vm1->host.sid,p_vm2->host.sid);
		if(DEBUG) printf("%s\n",p_vv->vmids);
	}

	/*����V2Vlist*/
	tot_v2v++;
	_gthis->sd->v2v_total=tot_v2v;	
}



/*���ܣ������׺ͣ�ͬ���׺�
����������float a,float b, ������С�ķ�Χ
          int affnum,   �׺ͱߵĸ���
		  int aftype    �׺�����
�����ߣ�CJH 2013.07.30
*/
void create_v2v_by_ta(float a,float b,int affnum,int aftype)
{
	int tot_v2v,tot_pm,tot_fr,tot_gr,i,fff,sel;
	
	srand( (unsigned)time( NULL ) ); 

	/*����׺ͱߵ�����*/
	tot_v2v=_gthis->sd->v2v_total;

	/*ѡ��VM*/
	switch(aftype){
		case 0:
			/*�������������ϵ����ͬһPM�ϵ�����VM֮�佨��������ϵ*/
			printf("ѡ��\n -1- �����������.\n");
			printf(" -2- ����VMID����.\n");
			scanf("%d",&sel);
			if(sel==1)
				for(i=0;i<affnum;i++) createOnev2v0(a,b);
			else
			{
				printf("��������VMID���������׺ͱߣ�");

			}

			break;
		case -3:/*��ͬ�飺
			��������ͬ����ѡȡ����PM��Ȼ��ֱ����ѡȡ����VM*/
			/*���������������ͬ���PM�ϵ�VM�����ܽ�����ͬ���׺͹�ϵ*/
			tot_gr=_gthis->sd->gr_total;
			
			if(tot_gr<2)
				printf("�����ݵ��鲻��2�����޷����ò�ͬ���׺ͣ�\n");
			else{
				/*������ɲ�ͬ���׺͹�ϵ*/
				printf("1-������ɣ�0-�ֹ�����");
				scanf("%d",&fff);
				for(i=0;i<affnum;i++) createOnev2v_3(fff);				
			}
			break;
		case -2:/*��ͬ��*/
			/*��������ͬ���PM��������VM����Ϊ��ͬ���׺�*/
			tot_fr=_gthis->sd->fr_total;
			
			if(tot_fr<2)
				printf("�����ݵĿ���2�����޷����ò�ͬ���׺ͣ�\n");
			else
				/*������ɲ�ͬ���ϵ*/
				for(i=0;i<affnum;i++) createOnev2v_2();				
		
			break;
		case -1:/*��ͬ��*/
			/*�������������ϵ*/
			tot_pm=_gthis->sd->pm_total;
			if(tot_pm<2)
				printf("�����ݵ�PM����2�����޷����ò�ͬ���׺ͣ�\n");
			else
				for(i=0;i<affnum;i++)	createOnev2v_1(-1);

			break;
		case 1:/*ͬ�壺����ѡȡһ��PM�ϣ�ȡ����VM*/
			for(i=0;i<affnum;i++) createOnev2v1();

			break;
		case 2:/*ͬ��:����ѡ�����ɸ�PM��������ͬPM*/
			for(i=0;i<affnum;i++) createOnev2v2();
			
			break;
		case 3:/*ͬ��*/
			for(i=0;i<affnum;i++) createOnev2v3();

			break;
	}
}

/***********************************************************************/
/*
* ������ : ifv2vrepeat(int src_id,int dst_id,int ID)
* ���� : ���������ɵ�vm2vm�Ƿ��ظ�
* ������� : int i,int j, int ID
* ����src_id : src_id��ԴvmID
* ����dst_id : dst_id��Ŀ��vmID
* ����ID : _gthis->sd->v2vlist
* ����ֵ : return 0��ʾ���ظ�, return 1��ʾ���ظ�
* @created by ��ҵ�� in 2013.5.12
*/
int ifv2vrepeat(int src_id,int dst_id, int ID)
{
	int i;
	/*��֮ǰ���ɵ�������ϵ���αȽ�,�����ظ���return 0*/
	for (i=0; i < ID-1; i++)
		if(_gthis->sd->v2vlist[i].p_dst_vm->host.id == dst_id 
			&& _gthis->sd->v2vlist[i].p_src_vm->host.id == src_id)
			return 0;

	return 1;
}

/*
* ������ : create_v2v_by_aff_type(float a, float b, int affnum, int afftype)
* ���� : �������vm������ϵ
* ������� :float a, float b, int affnum, int afftype
* ����lower : ��������
* ����upper : ��������
* ����affnum : ���ɱߵ�����
* ���� afftype:�׺����ͺ� -3~3
* ����ֵ : ��
* @created by �½��� in 2013.7.25
*/
void create_v2v_by_aff_type(float a, float b, int affnum, int afftype)
{
	/*3.�׺���*/
	/*printf("�����׺��Թ�ϵ��\n");
	printf("0��-�����׺��ԣ����ѡȡPM�������ϵ�VM֮�䴴����\n");
	printf("-3��-�����׺ͣ��ڲ�ͬ�������ѡȡ����PM�������ϵ�VM֮�䴴����\n");
	printf("-2��-�����׺ͣ��ڲ�ͬ�������ѡȡ����PM�������ϵ�VM֮�䴴����\n");
	printf("-1��-�����׺ͣ����ѡȡ����PM�������ϵ�VM֮�䴴����\n");
	printf("1��-ͬ���׺ͣ����ѡȡһ���飬�������ѡȡ����PM�������ϵ�VM֮�䴴����\n");
	printf("2��-ͬ���׺ͣ����ѡһ���򣬿������ѡȡ����PM�������ϵ�VM֮�䴴����\n");
	printf("3��-ͬ���׺ͣ����ѡȡһ��PM�������ϵ�VM֮�䴴����\n");

	printf("�����봴�����׺����ͱ�ţ���Ҫ����������,��0 10����ʾ���������׺���10���ߡ�\n�������������˳���\n");
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
* ������ : create_vm2vm (double lower, double upper, double total)
* ���� : �������vm������ϵ
* ������� : double lower, double upper, double total
* ����lower : ��������
* ����upper : ��������
* ����total : ���ɱߵ�����
* ����ֵ : ��
* @created by ��ҵ�� in 2013.5.12
*/
void create_v2v(double lower, double upper, int vmnum, double ratio)
{
	int src_id, dst_id,t_id;
	int total_side;	//�ܱ���
	int i;
	char vmids[500];

	total_side = (int)(vmnum * ratio);
	_gthis->sd->v2v_total = total_side;
	for (i = 0; i < total_side; i++){	//����total_side����
		src_id = (int) random_a2b(0,vmnum+1);//�������id
		dst_id = (int) random_a2b(0,vmnum+1);

		if (src_id == dst_id) dst_id++;

		if (src_id > dst_id) {//С��ID����src_id,����,edited by cjh 20130713
			t_id=src_id;
			src_id=dst_id;
			dst_id=t_id;
		}

		if (!ifv2vrepeat(src_id, dst_id, i)) i--;	//�жϱ��Ƿ��Ѿ�����

		_gthis->sd->v2vlist[i].id = i;/*�����±����*/

		_gthis->sd->v2vlist[i].p_vms[0]=&_gthis->sd->vmlist[src_id];
		_gthis->sd->v2vlist[i].p_vms[1]=&_gthis->sd->vmlist[dst_id];

		_gthis->sd->v2vlist[i].p_dst_vm = &_gthis->sd->vmlist[dst_id];
		_gthis->sd->v2vlist[i].p_src_vm = &_gthis->sd->vmlist[src_id];
		_gthis->sd->v2vlist[i].traffic = random_a2b(lower, upper);//�������������lower,upper��
		_gthis->sd->v2vlist[i].delay = (int)random_a2b(1,7);	//����1-6�ڵ���������������ڵ�ĸ���
		
		strcpy(vmids,_gthis->sd->v2vlist[i].p_vms[0]->host.sid); 
		strcat(vmids,",");
		strcat(vmids,_gthis->sd->v2vlist[i].p_vms[1]->host.sid); 
		strcpy(_gthis->sd->v2vlist[i].vmids,vmids);

		/*���������׺������� added by cjh 07/13/2013*/
		_gthis->sd->v2vlist[i].affinity =0; 

	}
}


/*����׺����Ƿ��ͻ*/
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
* ������ : void create_v2v_no_aff(int affnum��int afftype)
* ���� : �������vm���׺͹�ϵ, 
* ����ͬ��-һ��ͬ��ͬ�飬������ڲ�ͬ�塢��ͬ�򡢲�ͬ�飻
*	   ��ͬ��-������ͬ�壬������ͬ�飬����ͬ��
*      ͬ��-����ͬ�飬���Բ�ͬ�飬����ͬ�壬���Բ�ͬ�壬�����Բ�ͬ��
*      ͬ��-����ͬ�򣬿��Բ�ͬ�򣬿���ͬ�壬���Բ�ͬ�壬���� ��ͬ�飻
* ������� : int vmnum, int affnum
* ����affnum: ���ɵ�����
* ����afftype: �����׺�����-3��-2��-1��1,2,3
* ����ֵ : ��
* @created by ��ҵ�� in 2013.5.12
*/
void create_v2v_no_aff(int affnum, int afftype)
{
	int src_id, dst_id,t_id,vmnum,n;
	int v2v_tot=0;	//�ܱ���
	int i,j,k,flag;
	int vmid[100];/*ÿ���׺��鲻���Գ���100��id*/
	char vmids[500];
	vmnum=_gthis->sd->vm_total;
	v2v_tot=_gthis->sd->v2v_total;
	
	/*����total_side����*/
	for (i = v2v_tot; i < v2v_tot + affnum; i++){	
		if(afftype==-1 || afftype==-2 || afftype==-3){/*��ͬ�塢����*/
			src_id = (int) random_a2b(0,vmnum+1);//�������id
			dst_id = (int) random_a2b(0,vmnum+1);
		
			if (src_id == dst_id) dst_id++;

			if (src_id > dst_id) {//С��ID����src_id,����,edited by cjh 20130713
				t_id=src_id;
				src_id=dst_id;
				dst_id=t_id;
			}

			if (!ifv2vrepeat(src_id, dst_id, i)) i--;	//�жϱ��Ƿ��Ѿ�����
		
			
			/*�����׺������� added by cjh 07/23/2013*/
			_gthis->sd->v2vlist[i].affinity = afftype;
		
			_gthis->sd->v2vlist[i].id = i;/*�����±����*/
		
			_gthis->sd->v2vlist[i].p_vms[0]=&_gthis->sd->vmlist[src_id];
			_gthis->sd->v2vlist[i].p_vms[1]=&_gthis->sd->vmlist[dst_id];

			_gthis->sd->v2vlist[i].p_dst_vm = &_gthis->sd->vmlist[dst_id];
			_gthis->sd->v2vlist[i].p_src_vm = &_gthis->sd->vmlist[src_id];
		
			/*������������lower,upper��*/
			_gthis->sd->v2vlist[i].traffic =0;
		
			/*����1-6�ڵ���������������ڵ�ĸ���*/
			_gthis->sd->v2vlist[i].delay =0;	
		
			strcpy(vmids,_gthis->sd->v2vlist[i].p_vms[0]->host.sid); 
			strcpy(vmids,_gthis->sd->v2vlist[i].p_vms[1]->host.sid); 
			strcpy(_gthis->sd->v2vlist[i].vmids,vmids);

		} 
		else if(afftype==1) /*ͬ��*/
		{/*ͬ�塢ͬ��ͬ���׺�*/
			printf("����ͬ��vmids��vmid�ĸ�����");
			scanf("%d",&n);

			j=0;
			while(j<n){
				vmid[j]=(int) random_a2b(0,vmnum+1);
				
				if(j>0){
					/*�ж��Ƿ��Ѿ����ɹ���*/
					flag=0;
					for(k=0;k<j;k++)
						if(vmid[k]==vmid[j]) {flag=1;break;}

					if(flag==1) {continue;} /*���ظ�*/
					
					/*�Ƿ���ڷ��׺ͣ���ǰvmid��ǰ���Ѿ�����vmid���ɵ�ͬ���׺ͣ�
					�Ƿ���ڷ��׺ͣ�����У�����ֳ�ͻ����Ҫ��������vmid*/
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

		/*����+1*/
		_gthis->sd->v2v_total++;
	}
}



/*��������int getswnopmbypmid(int pmid)
* ���ܣ��ж��Ƿ���ù�
* ���������int pmid
* ���أ�   ���Է��õĽ�����id
* ������ �½��� 7.29
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
* ������ : create_pm_topo(void)
* ���� : ��������������ˣ�����PMnum,ȷ������Ľ���������(�������˿����涨Ϊ24��)
* ������� : double lower, double upper,int pmnum,int port
* ����1 ��ʱ�ӷ�Χ��pmnum-�����̨����port:�˿��� 
* ����ֵ : ��
* @created by �½��� in 2013.7.28
*/
void create_p2p_tree_topo(double lower, double upper,int pmnum)
{
	/*����PMnum,ÿ��PM����4���˿�ȷ������Ľ���������*/
	int i,j,k,ports,pport,level=0,start_swid;
	int tot_sw,tot_s2s,tot_p2s;
	int swnum0,swnum1; /*0-2���㽻��������*/
	pmPtr p_pm;
	swPtr psw,p_sw,p_sw1;
	pm2swPtr p_ps;
	sw2swPtr p_ss;
	st_swlink swlnk;
	
	/*---����������˿��뽻����0��˿ڵ�����-----------*/
	
	//tot_pm=_gthis->sd->pm_total;
	tot_p2s=_gthis->sd->p2s_total;

	/*ÿ���˿�����һ��������*/
	j=0;k=0;level=0;
	while(j<pmnum){
		/*ȡ��j��PM*/
		p_pm=&_gthis->sd->pmlist[j];
		
		/*Ϊpm��ÿ���˿ڽ���һ���뽻�����˿ڵ����ӣ�ÿ��PM�˿���Ҫ���벻ͬ�������Ķ˿�*/
		/*������ɸ�PM�Ķ˿���>=1,<=4*/
		ports=(int)(1+4*random_021());

		/*ÿ���˿���Ҫһ������������*/
		for(pport=0;pport<ports;pport++)
		{			
			/*����һ��p2s����*/
			p_ps=&_gthis->sd->p2slist[tot_p2s]; 
			
			/*PM��*/
			//p_pm->host.id=j;
			p_ps->id=tot_p2s;
			
			p_ps->pm_id=j;
			swlnk.id=j;
			swlnk.port=pport;
			swlnk.type=PMLINK;
			sprintf(p_ps->pm_sid,"pm%d",j);
			p_ps->port_id=pport;/*�˿ں�*/
			
			/*switch�� һ���˿�ֻ�ܽ�һ��������*/
			/*��һ��û�зŹ�pmid�Ľ�����*/
			psw=getswnopmbypmid(j);
			if(psw==NULL) /*���¿�һ��������*/
			{	tot_sw=_gthis->sd->sw_total;
				p_sw=&_gthis->sd->swlist[tot_sw];
				p_sw->id=tot_sw;
				p_sw->portnum=SW_PORT_NUM;/*�˿ڸ�����Χ*/
				p_sw->countoflinks=0;
				p_sw->level=level;
				sprintf(p_sw->sid,"sw%d",tot_sw);
		
				/*��������������1*/
				tot_sw++;
				_gthis->sd->sw_total=tot_sw;

			}else
				p_sw=psw;

			/*����һ������*/
			p_ps->switch_id=p_sw->id;
			p_ps->swithch_port=p_sw->countoflinks;
			sprintf(p_ps->switch_sid,"%s",p_sw->sid);
			p_ps->delay=1;//(float)(lower+(upper-lower)*random_021());
			p_ps->bandwidth=1024;
			

			/*���½�������������Ϣ*/
			p_sw->links[p_sw->countoflinks]=swlnk;
			p_sw->countoflinks++;
			
			tot_p2s++;/*p2s����1*/
		}

		j++;/*������һ��PM*/
	}

	
	_gthis->sd->p2s_total=tot_p2s;
	

	/*---���ɽ������뽻����֮�������--s2slist----*/
	tot_s2s=_gthis->sd->s2s_total;
	
	/*0���ÿ�����������ӵ�1����½����������ӹ�ϵ��¼��s2slist*/
	
	
	swnum0=tot_sw;/*430*/
	start_swid=0;

	swnum1=0;
	j=0;
	
	while(swnum0>1){
		
		level++;

		tot_sw=_gthis->sd->sw_total;

		for(i=0;i<swnum0;i++)/*0~429*/
		{
			/*ȡ0�㽻����*/
			
			p_sw=&_gthis->sd->swlist[start_swid+i];
		
			/*ȡ1�㽻����*/
			p_sw1=&_gthis->sd->swlist[tot_sw];
			if(i==0 && p_sw1->id>0)
			{
				swnum1++; tot_sw++;
				p_sw1=&_gthis->sd->swlist[tot_sw]; 
			}
			
			/*���˿�����û������*/
			while((p_sw1->portnum>0) && (p_sw1->countoflinks==p_sw1->portnum))
			{
				swnum1++; tot_sw++;
				p_sw1=&_gthis->sd->swlist[tot_sw]; 
			}

			/*�µ�SW:�½�һ��������*/
			if(p_sw1->id==0){
				/*���Ը���*/
				p_sw1->id=tot_sw;
				p_sw1->countoflinks=0;
				p_sw1->level=level;
				p_sw1->portnum=SW_PORT_NUM;
				sprintf(p_sw1->sid,"sw%d",tot_sw);
			}
		
			/*����һ����*/
			p_ss=&_gthis->sd->s2slist[tot_s2s];
		
			/*��������*/
			p_ss->id=tot_s2s;
			p_ss->p_src_sw=p_sw; /*���ӽ�����*/
			p_ss->sw_src_id=p_sw->id;
			sprintf(p_ss->sw_src_sid,"%s",p_sw->sid);

			p_ss->p_dst_sw=p_sw1;/*��һ�㸸�׽�����*/
		
			p_ss->delay=1;//(float)(lower+(upper-lower)*random_021());
			p_ss->bandwidth=1024;
		
			/*����������Ϣ*/
			swlnk.id=p_sw->id;
			swlnk.port=0;/*0�˿����п�*/
			swlnk.type=SWITCHLINK;/*��ʾ������*/
			p_sw1->links[p_sw1->countoflinks++]=swlnk;

			tot_s2s++;/*����SS���ӹ�ϵ������*/
		}
		tot_sw++;swnum1++;
		
		/*���½�����������*/
		//tot_sw+=swnum1;
		_gthis->sd->sw_total=tot_sw;
		/*����SS���ӹ�ϵ������*/
		_gthis->sd->s2s_total=tot_s2s;
		start_swid += swnum0;
		swnum0=swnum1;/*17*/
		swnum1=0;
	}

	_gthis->sd->s2s_total=tot_s2s;
}



/*
* ������ : create_p2p(void)
* ���� : �������ʱ�Ӿ���
* ������� : ��
* ����1 �� 
* ����ֵ : ��
* @created by ��ҵ�� in 2013.5.13
*/
void create_p2p(double lower, double upper,int pmnum)
{
	st_pm2pm * t_p2plist =_gthis->sd->p2plist;//����ָ��ָ��ȫ�ֱ���
	int src_id,dst_id;
	int t_total_pm;
	int i = 0;		//��¼p2plist��ID
	t_total_pm= pmnum;
	set_seed ();
	for (src_id = 0; src_id < t_total_pm; src_id ++) {//��src_id��0��pm����ѭ��
		for(dst_id = src_id + 1; dst_id < t_total_pm; dst_id ++){	//��dst_id��0��pm����ѭ��
			(t_p2plist+i)->pm_src_id = src_id;
			sprintf((t_p2plist+i)->pm_src_sid, "pm%d", src_id);
			(t_p2plist+i)->pm_dst_id = dst_id;
			sprintf((t_p2plist+i)->pm_dst_sid, "pm%d", dst_id);
			(t_p2plist+i)->pm_dst_port = (int) random_a2b(0,5);		//���ѡ������
			(t_p2plist+i)->pm_src_port = (int) random_a2b(0,5);
			(t_p2plist+i)->delay = random_a2b(0,7);				//�������ʱ��
			(t_p2plist+i)->bandwidth = random_a2b(lower, upper);	//������ɴ���
			i++;
			if(i >= P2P_TOTAL){
				LogMsg("overflow!\n");
				return;
			}
		}
	}
}

/*����:ͳ���ȵ������������*/
void count(void)
{
	int i,j,index;
	int hot,cold;
	printf("There are ");
	for(i=0,hot=0;i<_gthis->sd->pm_total;i++){//�������������,��һ��ά�ȴ���HOT����Ϊ�ȵ�
		for(j=0;j<DIM;j++)
			if(_gthis->sd->pmlist[i].host.load[j].used / _gthis->sd->pmlist[i].host.load[j].total > HOT){
				hot++;
				break;
			}
	}
	printf("%d hot pm.\n",hot);//����ȵ������̨��

	printf("There are ");
	for(i=0,cold=0;i<_gthis->sd->pm_total;i++){//�������������,������ά��С��COLD����Ϊ���
		for(j=0,index=1;j<DIM;j++)
			if(_gthis->sd->pmlist[i].host.load[j].used / _gthis->sd->pmlist[i].host.load[j].total > COLD){
				index = index&&0;
				break;
			}
			if(index&&_gthis->sd->pmlist[i].host.stat)
				cold++;
	}
	printf("%d cold pm.\n",cold);//�����������̨��

	printf("There are ");//����������̨��
	for(i=0,index=0;i<_gthis->sd->pm_total;i++){
		if(!_gthis->sd->pmlist[i].host.stat)
			index++;
	}
	printf("%d empty pm.\n",index);
}

/*���ǿ��������stat��Ϊ1,���������Ϊ0*/
void set_stat(void)
{
	int i,j;
	int index;
	for(i=0;i<_gthis->sd->pm_total;i++){			//�������������,�������������stat��Ϊ0
		for(j=0,index=1;j<DIM;j++)
			if(_gthis->sd->pmlist[i].host.load[j].used)
				index = index&&0;
		if(!index)
			_gthis->sd->pmlist[i].host.stat = 1;
	}
}