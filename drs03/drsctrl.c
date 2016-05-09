
#include "drsctrl.h"



/************************************************/
/*initial the global data struc.*/
/******************************************/
void init_shared_data()
{
	int i;
	char *nMet[8]= {"cpu", "mem", "diskio", "fpga", 
	"eth0", "eth1", "eth2", "eth3"};

	for(i=0;i<8;i++)
	{
		strcpy(nameOfMetric[i],nMet[i]);	
	}

	memset(g_vmlist, 0, sizeof(g_vmlist));//��ʼ��
	memset(g_pmlist, 0, sizeof(g_pmlist));//��ʼ��
	memset(g_pmglist, 0, sizeof(g_pmglist));//��ʼ���������
	memset(g_framelist, 0, sizeof(g_framelist));//��ʼ����
	
	/*���˽ṹ�� 07.28 CJH ADD*/
	memset(g_swlist, 0, sizeof(g_swlist));//��ʼ��g_swlist
	memset(g_s2slist, 0, sizeof(g_s2slist));//��ʼ��g_s2slist
	memset(g_p2slist, 0, sizeof(g_p2slist));//��ʼ��g_p2slist
	
	memset(g_v2vlist, 0, sizeof(g_v2vlist));//��ʼ�������������ϵ
	memset(g_p2plist, 0, sizeof(g_p2plist));//��ʼ�������·�������ϵ
	memset(g_v2plist, 0, sizeof(g_v2plist));//��ʼ���������������Ĳ�����Ϣ
	memset(g_vmglist, 0, sizeof(g_vmglist));//��ʼ�����������
	memset(g_actionlist, 0, sizeof(g_actionlist));//��ʼ��
	memset(g_reqlist, 0, sizeof(g_reqlist));//��ʼ��
	memset(shareStr, 0, sizeof(shareStr));

	memset(g_sg_afflist,0,sizeof(g_sg_afflist));
	{
		int i;
		for (i = 0;i <DIM; i++)
			g_sharedata.weight[i] = 1;
	}
	
	g_sharedata.fr_total=0;
	g_sharedata.gr_total=0;
	g_sharedata.p2p_total=0;
	g_sharedata.p2s_total=0;
	g_sharedata.s2s_total=0;
	g_sharedata.sw_total=0;
	g_sharedata.action_total=0;
	g_sharedata.vmg_total=0;
	g_sharedata.pm_total=0;
	g_sharedata.vm_total=0;
	g_sharedata.sg_afflist_total = 0;
	g_sharedata.afflist_total = 0;
	g_sharedata.isFirstPlace = 0;
	g_sharedata.readFailed = 0;
	g_sharedata.timeLimit = 10;
	g_sharedata.singleTimeLimit = 1;
	g_sharedata.maxTryTimesForHot = 10;
	g_sharedata.lastPlacePM = -1;
	g_sharedata.singleDFScount = 10000000;
	g_sharedata.sysNonEmptyPM = -1;
	


	g_sharedata.shareStrPtr = shareStr;
	g_sharedata.reqlist=g_reqlist;
	g_sharedata.vmlist=g_vmlist;
	g_sharedata.pmlist=g_pmlist;
	g_sharedata.pmglist=g_pmglist;
	g_sharedata.framelist=g_framelist;
	g_sharedata.v2vlist=g_v2vlist;
	g_sharedata.afflist=g_afflist;
	g_sharedata.sg_afflist = g_sg_afflist;
	g_sharedata.p2plist=g_p2plist;
	g_sharedata.v2plist=g_v2plist;
	g_sharedata.vmglist=g_vmglist;
	g_sharedata.actionlist=g_actionlist;

	g_sharedata.swlist=g_swlist;
	g_sharedata.s2slist=g_s2slist;
	g_sharedata.p2slist=g_p2slist;
	
	return;
}
	

/*����XML1, XML2,XML21,XML22*/
void loaddata(char *rootpath)
{
	char url_input_file[200];
	printf("Xml datas locate in %s:\n",rootpath);
				
	/*1.---��ʼ��������*/
	init_drscontrol(&dc);
	/*��ʼ����������ָ��*/
	dc.sd=&g_sharedata;
	
	/*2.---����XML1�ļ��������������������requestlist*/
	//�������������Ϣ�ļ�
	g_sharedata.req_total=0;
	printf("\nRead xml1.xml:\n-------------------------\n");
	strcpy(url_input_file,rootpath);
	strcat(url_input_file,"xml1.xml");
	dc.parseXML1(url_input_file);
	printf("---END XML1----------------\n\n");
	
	dc.setDefaultRequest();


	/*3.---����VM����XML�ļ�*/
	/*����XML2�ļ��������������������pmlist\vmlist*/
	printf("\nRead xml2.xml:\n-------------------------\n");
	strcpy(url_input_file,rootpath);
	strcat(url_input_file,"xml2.xml");
	dc.parseXML2(url_input_file);
	if (dc.sd->readFailed == 1){
		return ;
	}
	/*����*/
	if(DEBUG) printf("\n\nPM������%d��VM������%d����ĸ�����%d,��ĸ�����%d",
		dc.sd->pm_total,dc.sd->vm_total,dc.sd->fr_total,dc.sd->gr_total);
	
	printf("\n---END XML2----------------\n\n");

	/*/����PM2PM֮���ʱ�ӹ�ϵ�ļ�*/
	printf("\nRead xml21.xml:\n-------------------------\n"); 
	strcpy(url_input_file,rootpath);
	strcat(url_input_file,"xml21.xml");
	dc.parseXML21(url_input_file);
	//xmlread.get_pm2pm_from_xml21(url_input_file);
	printf("---END XML21----------------\n\n");
	
	
	/*����VM֮������ӹ�ϵ�ļ�*/
	printf("Read xml22.xml:\n-------------------------\n");
	strcpy(url_input_file,rootpath);
	strcat(url_input_file,"xml22.xml");
	dc.parseXML22(url_input_file);
	printf("---END XML22----------------\n\n");
	


	printf("Load data from %s sucessfully!\n",rootpath);
}


/*��ѯ����*/
int searchdata()
{
	int seltp=1,selt;
	int i,j;
	pmPtr ppm;
	framePtr pfr;
	pmgPtr ppmg;
	vmPtr pvm;

	while(seltp>0){
		printf("����������ͣ�");
		printf("\n--------------------------------------------------------\n");
		printf("\n0-����\n1-PM\n2-VM\n3-group\n4-frame\n5-vm2vm\n6-vm2pm\n");
		printf("\n--------------------------------------------------------\n������:");
		scanf("%d",&seltp);
		switch(seltp){
		case 0:
			return 0;
			break;
		case 1:/*������PM*/
			printf("0-�г�����PM��1-����PMid�г���PM��VM\n�����룺");

			scanf("%d",&selt);
			if(selt==0){
				for(i=0;i<g_sharedata.pm_total;i++)
				{
					ppm=&g_sharedata.pmlist[i];
					printf("[%d,%s]",i,ppm->host.sid);
					if((i+1) % 8 ==0) printf("\n");
				}
			}
			if(selt==1)
			{
				int pmid;
				printf("����PMID����Χ��0--%d��\n",g_sharedata.pm_total);
				scanf("%d",&pmid);
				ppm=&g_sharedata.pmlist[pmid];
				printf("[%d,%s]\n",pmid,ppm->host.sid);
				printf("%d vms in this pm:\n(id[host_id,host_sid])\n",ppm->countofvm);
				for (j=0;j<ppm->countofvm;j++)
				{
					pvm=ppm->p_vms[j];
					printf("%d[%d,%s] ",j,pvm->host.id,pvm->host.sid);
					if((j+1)%8==0) printf("\n");
				}
			}
			printf("\n--------------------------------------------------------\n");
			break;
		case 2:/*������VM*/
			printf("0-�г�����VM��1-����VMid�г���VM����Ϣ.\n�����룺");
			scanf("%d",&selt);
			if(selt==0){
				printf("[vmid,sid,stat,at,load]\n");
				for(i=0;i<g_sharedata.vm_total;i++)
				{
					pvm=&g_sharedata.vmlist[i];
					printf("[%d,%s,%d,%d,",i,pvm->host.sid,pvm->host.stat,pvm->at);
					for(j=0;j<DIM;j++){
						printf(",%8.3f",pvm->host.load[j].total);
					}
					printf("]\n");
					//if(i % 2 ==0) printf("\n");
				}
			}
			if(selt==1)
			{
				int vmid;
				printf("����һ��VMID����Χ��0--%d��\n",g_sharedata.vm_total);
				scanf("%d",&vmid);
				pvm=&g_sharedata.vmlist[vmid];
				printf("[%d,%s,%d,%d,",vmid,pvm->host.sid,pvm->host.stat,pvm->at);
				for(j=0;j<DIM;j++)
				{
					printf(",%8.3f",pvm->host.load[j].total);
				}
				printf("]");
			}
			printf("\n--------------------------------------------------------\n");
			break;
		case 3:/*��PM group*/
			printf("0-�г�����PM GROUP��1-����PMGid�г���PMG��PM\n�����룺");

			scanf("%d",&selt);
			if(selt==0){

				for(i=0;i<g_sharedata.gr_total;i++)
				{
					ppmg=&g_sharedata.pmglist[i];
					printf("pm group:[%d,%s] \n pms:[",i,ppmg->sid);
					for(j=0;j<ppmg->countofpm;j++)
					{
						ppm=&g_sharedata.pmlist[ppmg->i_pmids[j]];
						printf("(%d,%s)",j,ppm->host.sid);
					}
					printf("]\n");
					//if((i+1) % 8 ==0) printf("\n");
					
				}
			}
			if(selt==1)
			{
				int pmgid;
				printf("����PMGID����Χ��0--%d��\n",g_sharedata.gr_total);
				scanf("%d",&pmgid);
				ppmg=&g_sharedata.pmglist[pmgid];
				printf("[%d,%s]\n",pmgid,ppmg->sid);
				printf("%d pms in this pmgroup:\n(id[host_id,host_sid])\n",ppmg->countofpm);
				for (j=0;j<ppmg->countofpm;j++)
				{
					ppm=&g_sharedata.pmlist[ppmg->i_pmids[j]];
					printf("%d[%d,%s] ",j,ppm->host.id,ppm->host.sid);
					if((j+1)%8==0) printf("\n");
				}
			}
			printf("\n--------------------------------------------------------\n");
			break;
		case 4:/*��PM frame*/
			printf("0-�г�����PM frame��1-����frameid�г���frame��PM\n�����룺");

			scanf("%d",&selt);
			if(selt==0){

				for(i=0;i<g_sharedata.fr_total;i++)
				{
					pfr=&g_sharedata.framelist[i];
					printf("frame:[%d,%s] \n pms:[",i,pfr->sid);
					for(j=0;j<ppmg->countofpm;j++)
					{
						ppm=&g_sharedata.pmlist[pfr->i_pmids[j]];
						printf("(%d,%s)",j,ppm->host.sid);
					}
					printf("]\n");
					//if((i+1) % 8 ==0) printf("\n");
					
				}
			}
			if(selt==1)
			{
				int frid;
				printf("����FRAMEID����Χ��0--%d��\n",g_sharedata.fr_total);
				scanf("%d",&frid);
				pfr=&g_sharedata.framelist[frid];
				printf("[%d,%s]\n",frid,pfr->sid);
				printf("%d pms in this frame:\n(id[host_id,host_sid])\n",pfr->countofpm);
				for (j=0;j<pfr->countofpm;j++)
				{
					ppm=&g_sharedata.pmlist[pfr->i_pmids[j]];
					printf("%d[%d,%s] ",j,ppm->host.id,ppm->host.sid);
					if((j+1)%8==0) printf("\n");
				}
			}
			printf("\n--------------------------------------------------------\n");
			break;
		case 5:/*�����е��׺���*/
			printf(" id affinity traffice delay vmids at\n");				
			printf("--------------------------------------------------------\n");
			for(i=0;i<g_sharedata.v2v_total;i++)
			{
				vm2vmPtr pvv;
				pvv=&g_sharedata.v2vlist[i];
				printf("%3d %3d %6.2f %5.2f %s %s\n",
					pvv->id,pvv->affinity,
					pvv->traffic,pvv->delay,
					pvv->vmids,pvv->at);				
			}
			printf("\n--------------------------------------------------------\n");
			break;
		case 6:
			
			break;
		}
	}
}



/*���ܣ�����ģ������ 7.25 �½����ύ*/
void GenerateData(char *rootdir)
{
	/*��ʼ��ȫ���������ݽṹ*/
	cls_st_datagenerator dg;
	cls_st_drscontrol dc;
	char url[200];
	int vnum,pnum,grnum,afnum,emptypmnum,n;
	float a,b,htv;
	int choice,j,sel;
	int from;

	srand((unsigned)time(NULL)); 

	if(g_sharedata.pm_total>0){
		printf("���ݲ�Ϊ�գ��Ƿ񸲸��������ɣ�\n ѡ��-1- ����; -0- ����׷������.\n ���룺");
		scanf("%d",&sel); 
		/*��ʼ��*/
		if(sel==1) init_shared_data();	
		
	}else
	{
		init_shared_data();
	}
	
	/*����ģ������*/
	init_drscontrol(&dc);
	/*��ʼ����������ָ��*/
	dc.sd=&g_sharedata;
	/*1.��������ģ�����ݵĺ�����������������global��*/
	
	init_generator(&dg);
	dg.sd=&g_sharedata;
	
	/*��ʼ���칹��������*/
	dg.createPMV();
	
	printf("Now we generate simulation data:\n");
	
	sel=1;
	while(sel>0){
		printf("��ѡ��\n");
		printf("----------------------------------------------------\n");
		printf(" -1- ����VM��PM���ݣ�\n");
											/*1.�������VM��PM��
											2.��ָ��PM���VM��
											3.��ӿ���PM��
											4.������PMΪ���أ���Գ�ʼ�������ݣ�
											*/
		printf(" -2- ���ɿ����ݣ�\n");
										/*1.���ڸ���PM�Զ����ɿ�
										2.��Ӹ������Ŀ�PM��
										3.����������PM��
										4.�������޸�PM��*/
		printf(" -3- ���������ݣ�\n");
										/*1.���ڸ���PM�Զ������飻
										2.��Ӹ������Ŀ�PM�飻
										3.�����PM�����PM��
										4.����PM���޸�PM��*/
		printf(" -4- �����׺������ݣ�\n");
		
		printf(" -5- ����������������; \n");

		printf(" -6- ����XML����.\n "); /*1.����XML2; 
										  2.����XML21. 
										  3.����XML22.
										  0.����*/
		printf("-0- ����.\n");
		printf("----------------------------------------------------\n");
		printf("���룺");

		scanf("%d",&sel);

		switch(sel){
		case 1:
			/*1.����PM��VM����*/
			Generate_PMVM();
			break;
		case 2:
			/*2.���ɿ�*/
			Generate_FRAME();
			break;
		case 3:
			/*3.������*/
			Generate_PMG();
			break;
		case 4:
			/*4.�����׺���*/
			Generate_AFF();
			break;
		case 5:/*-5- ����������������;*/
			Generate_TOPO();
			break;
		case 6:/*1.����XML2;  2.����XML21. 3.����XML22.*/
			Generate_XML(rootdir);
			break;
		}
	}

	printf("ģ������������ϣ���0-���أ�\n����:\n");
}


/*�����ݱ��浽XML�ļ�,������XML*/
void Generate_XML(char *rootdir)
{	int sel;
	char url[200];
	
	cls_st_drscontrol dc;
	init_drscontrol(&dc);
	dc.sd=&g_sharedata;

	printf("ѡ��\n -1- ����XML2; \n");
	printf(" -2- ����XML21.\n"); 
	printf(" -3- ����XML22.\n");
	printf(" �����룺\n");
	scanf("%d",&sel);
	
	if(sel==1){/*�����XML2*/
		strcpy(url,rootdir);
		strcat(url,"xml2.xml");
		dc.constructXML2(url);
		printf("XML2[VM-PM]���ݴ����ɹ�����鿴XML�ļ���%s��\n",url);
	}
	else if(sel==2){
		/*Generate XML22*/
		strcpy(url,rootdir);
		strcat(url,"xml21.xml");
		dc.constructXML21(url);
		printf("PM�������ݴ����ɹ�����鿴Ŀ¼%s�е�XML�ļ���\n",url);
	}
	else if(sel==3)
	{
		/*Generate XML22*/
		strcpy(url,rootdir);
		strcat(url,"xml22.xml");
		dc.constructXML22(url);
		printf("�׺��Թ�ϵ���ݴ����ɹ�����鿴Ŀ¼%s�е�XML�ļ���\n",url);
	}
}

/*������*/
/*
1.���ڸ���PM�Զ������飻
2.��Ӹ������Ŀ�PM�飻
3.�����PM�����PM��
4.����PM���޸�PM��*/
void Generate_PMG()
{
	int i,j,x,choice=1,grnum,from,pnum,tot;
	sharedataPtr sd;
	pmgPtr p_pmg;
	st_pmgroup pmg; 
	cls_st_datagenerator dg;

	init_generator(&dg);
	dg.sd=&g_sharedata;

	while(choice>0)
	{
		printf("ѡ��\n");
		printf(" -1- ��������PM������ÿ��PM���PM̨�����Զ��������PM��.\n");
		printf(" -2- ��������PM��ŵ�PM��.\n");
		printf(" -3- ��Ӹ������Ŀ�PM��.\n");
		printf(" -4- ���ڸ����Ķ��PMID������һ����PM��.\n");
		printf(" -5- ���¸���PM���PMID��Ϣ����������Ķ��PMID��\n");
		printf("     ��1-5��8,9,��ʾ��pm1-pm5,pm8,pm9���������һ��PM��).\n");
		printf(" -6- ��ʾ����PM���б�.\n");
		printf(" -0- ����.\n");
		
		printf("===============================\n");
		printf("����:");
		scanf("%d",&choice);
		if(choice==0) break;
	
		switch(choice){
		case 1:/*-1- ��������PM������ÿ��PM���PM̨�����Զ��������PM��.\n*/
			printf("��ʵ��......\n");
			break;
		case 2:/*��������PMID��PM��*/
			printf("���룺PM�����,PM��ʼID�ţ�������ŵ�PM������ʽ��:\n");
			printf("---------------------------------------------------------\n");
			printf(" 3          ��ʾҪ����3����.\n");
			printf(" 0 5        ��1�飺PMID�� 0��ʼ������5��PM����һ��PM��\n");
			printf(" 5 5        ��2�飺PMID�� 5��ʼ������5��PM����һ��PM��\n");
			printf("10 5        ��3�飺PMID��10��ʼ������5��PM����һ��PM��\n");
			printf("---------------------------------------------------------\n");
			printf("����:");
			scanf("%d",&grnum);
			for(j=0;j<grnum;j++){
				scanf("%d%d",&from,&pnum);
				init_generator(&dg);
				dg.create_group(from,pnum);
			}
			break;
		case 3:/*��Ӹ������Ŀ�PM��.*/
			printf("������PM�������");
			scanf("%d",&grnum);
			for(i=0;i<grnum;i++){
				p_pmg=&sd->pmglist[dg.sd->gr_total];
				p_pmg->countofpm=0;
				p_pmg->id=dg.sd->gr_total;
				sprintf(p_pmg->sid,"pmg%d",p_pmg->id);
				dg.sd->gr_total++;
			}
			break;
		case 4:/* -4- ���ڸ����Ķ��PMID������һ����PM��.*/
			printf("����PM�Ķ��ID����-1�������ÿո�ֿ���\n");
			printf("�磺1 2 3 4 5 -1����ʾ����ID Ϊ1 2 3 4 5��5��PM����һ��PM��.\n");
			printf("----------------------------------------\n");
			printf("���룺");
			tot=g_sharedata.gr_total;

			p_pmg=&g_sharedata.pmglist[tot];
			p_pmg->countofpm=0;

			i=0;
			while(1){
				pmPtr ppm;
				scanf("%d",x); 
				if(x==-1) break;
				ppm=&g_sharedata.pmlist[x];
				if(ppm->host.id>=0){
					/*��Ҫ�жϸ�pmID�����µı��*/
					/*
					if(p_pmg->countofpm==0)
					{
						p_pmg->i_pmids[p_pmg->countofpm++]=ppm->host.id;
						sprintf(p_pmg->pmids,"%s",ppm->host.sid);
					}
					else
						sprintf(p_pmg->pmids,"%s,%s",p_pmg->pmids,ppm->host.sid);*/
				}
			};
			p_pmg=&g_sharedata.pmglist[tot]; 
			/*���뵽pmglist*/
			dg.sd->gr_total++;

			//printf("��ʵ��......\n");
			break;
		case 5:/*���¸���PM���PMID��Ϣ����������Ķ��PMID��*/
			printf("��ʵ��......\n");
			break;
		case 6:
			showpmglist();
			//printf("��ʵ��......\n");
			break;
		}
		printf("====================================\n");
		printf("�ɹ������飬�ۼ�����Total groups:%d:.\n",dg.sd->gr_total);
	}

}


/*���ɿ�*/
void Generate_FRAME()
{
	int choice=1,pnum;
	cls_st_datagenerator dg;
	init_generator(&dg);
	dg.sd=&g_sharedata;

	while(choice>0){
		printf("ѡ��\n -1- ���ݸ���ÿ����PM̨�����Զ����ɿ���Ϣ.\n");
		printf(" -2- �������PMID����̬����һ���¿�.\n");
		printf(" -3- �޸ĸ�������������Ķ��PMID����1-5 8 9,\n");
		printf("     ��ʾ��pm1-pm5,pm8,pm9���������һ����).\n");
		printf(" -4- ��ʾ���п��б�.\n");
		printf(" -0- ����.\n");
		
		printf("===============================\n");
		printf("����:");
		scanf("%d",&choice);
		if(choice==0) break;

		switch(choice){
		case 1:
			printf("����һ�����PM̨��):");
			scanf("%d",&pnum);
			dg.create_frame(pnum);
			break;
		case 2:
			printf("��ʵ��......\n");
			break;
		case 3:
			printf("��ʵ��......\n");
			break;
		case 4:
			showframelist();
			/*printf("��ʵ��......\n");*/
			break;
		}

		printf("�ɹ��������ۼ�����Total Frame:%d:.\n",dg.sd->fr_total);
		printf("---------------------------------------------------------\n");
	}
}

/*��PMΪ����
printf("ѡ��:\n -1- ������PMΪ����.\n");
printf("-2- �����ID���øÿ��ڵ�PMΪ����.\n");
printf("-3- ������ID���ø����ڵ�PMΪ����.\n");
printf("-4- ����PMID���ø�PMΪ����.");
printf("-0- ����.");*/
void SetEMPTYPM(int sel)
{
	int frid,grid,pnum,i,j,k;
	pmPtr pmlist,ppm;
	st_pm lstpm[100];

	cls_st_datagenerator dg;
	init_generator(&dg);
	dg.sd=&g_sharedata;

	if(sel==0) return;

	switch(sel){
	case 1:/*������PMΪ����*/
		pmlist=g_sharedata.pmlist;
		dg.set_empty_pm(pmlist,g_sharedata.pm_total);
		printf("�ɹ�����%d̨PMΪ�ա�\n",g_sharedata.pm_total);
		break;
	case 2:
		printf("�����ID��");
		scanf("%d",&frid);
	
		/*��������PM����ȡ��ID��Ӧ������PM��lstpm����*/
		j=0;
		for(i=0;i<g_sharedata.pm_total;i++)
		{
			ppm=&g_sharedata.pmlist[i];
			if(ppm->at_p_fr->id==frid) lstpm[j++]=*ppm;  
		}

		pnum=j;
		dg.set_empty_pm(lstpm,pnum);

		break;
	case 3:
		printf("������ID��");
		scanf("%d",&grid);
	
		/*��������PM����ȡ��ID��Ӧ������PM��lstpm����*/
		j=0;
		for(i=0;i<g_sharedata.pm_total;i++)
		{
			ppm=&g_sharedata.pmlist[i];
			for(k=0;k<ppm->countofpmgrp;k++)
				if(ppm->at_p_pmgrps[k]->id==grid)
				{
					lstpm[j++]=*ppm;
					break;
				}  
		}

		pnum=j;
		dg.set_empty_pm(lstpm,pnum);

		//printf("-3- ������ID���ø����ڵ�PMΪ����.\n");
		//printf("��ʵ��......");
		break;
	case 4:

		break;
	}


}


void Generate_EPTPM()
{
	int j,sel=1,pnum,vnum,n;
	float a,b,htv;
	cls_st_datagenerator dg;

	init_generator(&dg);

	printf("�Ƿ����ӿ���PM?�����룺1-���ӣ�0-���ԣ� �س���\n���룺");
	/*���PM*/
	scanf("%d",&sel);
	while(sel>0)
	{
		switch(sel)
		{
		case 1:
			printf("���������PM����N����������ǰ���ۼ����ɵ�PM��һ��ճ�һ����������82��+������118=200��.\n���룺");
			scanf("%d",&pnum); /*̨��*/
			dg.create_empty_pm(pnum);
			printf("�ۼ�total PM:%d.\n",dc.sd->pm_total);
			
			break;
		case 2:
			printf("������ʽ�磺\n");
			printf("3                        ----------��ʾ3�ֹ��\n");
			printf("100 0.0 0.1 0.2          ---------��ʾ��һ����0.2�����\n");
			printf("30 0.5 0.8 0.9           ----------��ʾ�ڶ�����0.9���ȵ����Ҫ��\n");
			printf("100 0.0 0.3 0.7          ---------��ʾ��������0.7���µ�\n-------------------------------------\n");
			printf("���룺");
			scanf("%d",&n);
		
			j=0;
			while(j<n){
				scanf("%d",&vnum); /*VM��*/
				scanf("%f%f%f",&a,&b,&htv);/*���ط�Χ�����ȶ�*/
				/*1.�����������VM��FF���ԣ���������ģ�����ݺ���*/
				dg.generate_data_by_vms(vnum,a,b,htv);
	
				printf("Total VM:%d, total PM:%d.\n",dc.sd->vm_total,dc.sd->pm_total);
				j++;
			}
			break;
		}
		
		printf("�Ƿ�������ӿ���PM?��1-���ӣ�0-���ԣ��س���\n���룺");
		scanf("%d",&sel);
	}
		
}


/*����PMVM*/
/*1.�������VM��PM��
2.��ָ��PM���VM��
3.��ӿ���PM��
4.������PMΪ���أ���Գ�ʼ�������ݣ�
*/
void Generate_PMVM()
{
	int i,j,n,vnum,pnum,sel=1,opt;
	float a,b,htv;
	cls_st_datagenerator dg;
	init_generator(&dg);
	dg.sd=&g_sharedata;

	while(sel>0){
		printf("��ѡ��\n -1- �������VM��PM.\n");
		printf(" -2- ��ָ��PM���VM.\n");
		printf(" -3- ��ӿ���PM.\n");
		printf(" -4- ��PMΪ���أ���Գ�ʼ�������ݣ�.\n");
		printf(" -0- ����.\n");
		printf("����:");
	
		scanf("%d",&sel);
		if(sel==0) break;

		switch(sel){
		case 1:
			printf("--------------------------------------------------------\n");
			printf("���룺\n ���ݹ����N,��ʾҪ����N�ֹ������ݡ�\n");
			printf(" ÿ�ֹ���4��������\n VM̨������ʾ�����û�Ҫ���ɵ�VM����\n ");
			printf(" ����0��1��������ʾVM����ϵ����Χ������ֹ��(��0 0.2����ʾ��0��0.2֮��),\n");
			printf(" ������¶�(0~1����ʾ��VMװPM�󽫴ﵽ���¶�).\n");
	
			printf("�����ʽ���£�\n");
			printf("3                       ----------��ʾ3�ֹ��\n");
			printf("100 0.0 0.1 0.2          ---------��ʾ��һ����\n");
			printf("30 0.5 0.8 0.9          ----------��ʾ�ڶ�����\n");
			printf("100 0.0 0.3 0.7          ---------��ʾ��������\n");
			printf("-------------------------------------\n");
			printf("���룺");
			scanf("%d",&n);
		
			j=0;
			while(j<n){
				scanf("%d",&vnum);
				scanf("%f%f%f",&a,&b,&htv);/*���ط�Χ�����ȶ�*/
				/*1.�����������VM��FF���ԣ���������ģ�����ݺ���*/
				dg.generate_data_by_vms(vnum,a,b,htv);
			
				printf("�ۼ�����Total VM:%d, total PM:%d.\n",dg.sd->vm_total,dg.sd->pm_total);
				j++;
			}

			break;
		case 2:/* -2- ��ָ��PM���VM.*/
			/*ѡ������ָ��PM�����룬�г���PM�ڵ�����VM��
			������Ҫ�����¿���VM̨����*/
			{
				int pn,vn;
				scanf("%d%d",&pn,&vn);/*����PM̨��,VM̨��*/
				//scanf();
			}
			break;
		case 3:
			//Generate_EPTPM();/*��ӿ���PM*/
			printf("����:Ҫ���ɵĿ���PM����N����������ǰ���ۼ����ɵ�PM��һ��ճ�һ��������\n");
			printf(" ��82��+������118=200��.\n���룺");
			scanf("%d",&pnum); /*̨��*/
			dg.create_empty_pm(pnum);
			printf("�ۼ�total PM:%d.\n",dg.sd->pm_total);
			
			break;
		case 4:
			/*���PM����*/
			opt=1;
			while(opt>0){
				printf("ѡ��:\n -1- ������PMΪ����.\n");
				printf("-2- �����ID���øÿ��ڵ�PMΪ����.\n");
				printf("-3- ������ID���ø����ڵ�PMΪ����.\n");
				printf("-4- ����PMID���ø�PMΪ����.\n");
				printf("-0- ����.\n ����:");
				
				scanf("%d",&opt);
				if(opt==0) break;
				if(opt==1)
					SetEMPTYPM(opt);
			}
			break;
		}

	}

}



//	printf("1.����PM��VM����:Ҫ������VM���������ã���PM�����������أ���VM����ϵ�������ȶ�\n");
//	
//	/*֧�ֶೡ������*/
//	printf("��ѡ����Ⱦ������ͣ�\n -1- ��ʼ���ã�\n -2- ��̬���ȣ����ؾ��⣻\n");
//	/*printf("   3-��̬����-�ȵ�����;\n   4-��̬����-�������.\n   0-�˳���\n-----------------\n���룺");*/
//
//	printf("   3-��̬����-�ȵ�����;\n   4-��̬����-�������.\n   0-�˳���\n-----------------\n���룺");
//	
//	Generate_EPTPM();
//
//	scanf("%d",&sel);
//	switch (sel)
//	{
//	case 1:/*��ʼ��������:�����������ȶȣ�<0.2,<0.7,<0.9*/
//			
//		//scanf("%f%f%f",&a,&b,&htv);
//		
//
//		break;
//	case 2:/*���ؾ���*/
//		printf("����������1������ȫ���ݣ���2����PM���غ����ӿ���PM��\n");
//		printf("(1)������һ�����ݹ������N,��ʾҪ���ɼ������͵����ݵ�ͬһ���ļ���\n");
//		printf("ÿһ�ֹ�����4��������1��VM���������ã�,2��3��VM����ϵ����Χ������ֹ��(��0 0.2����ʾ��0��0.2֮��),\n4��������ȶ�ϵ��(0~1����ʾPM�¶�Խ��).\n");
//	
//		printf("������ʽ�磺\n");
//		printf("3                       ----------��ʾ3�ֹ��\n");
//		printf("100 0.0 0.1 0.2         ----------��ʾ��һ����\n");
//		printf("30 0.5 0.8 0.9          ----------��ʾ�ڶ�����\n");
//		printf("100 0.0 0.3 0.7         ----------��ʾ��������\n------------------------------------\n");
//		printf("���룺");
//		
//		scanf("%d",&n);
//		j=0;
//		while(j<n){
//			scanf("%d",&vnum); /*VM��*/
//			scanf("%f%f%f",&a,&b,&htv);/*���ط�Χ�����ȶ�*/
//			/*1.�����������VM��FF���ԣ���������ģ�����ݺ���*/
//			dg.generate_data_by_vms(vnum,a,b,htv);
//	
//			printf("Total VM:%d, total PM:%d.\n",dc.sd->vm_total,dc.sd->pm_total);
//			j++;
//		}
//
//
//		break;
//	case 3:/*�ȵ�����*/
//		
//		printf("����������1������ȫ���ݣ���2����PM���غ����ӿ���PM��\n");
//		printf("(1)������һ�����ݹ������N,��ʾҪ���ɼ������͵����ݵ�ͬһ���ļ���\n");
//		printf("ÿһ�ֹ�����4��������1��VM���������ã�,2��3��VM����ϵ����Χ������ֹ��(��0 0.2����ʾ��0��0.2֮��),\n4��������ȶ�ϵ��(0~1����ʾPM�¶�Խ��).\n");
//	
//		printf("������ʽ�磺\n");
//		printf("3                        ----------��ʾ3�ֹ��\n");
//		printf("100 0.0 0.1 0.2          ---------��ʾ��һ����0.2�����\n");
//		printf("30 0.5 0.8 0.9           ----------��ʾ�ڶ�����0.9���ȵ����Ҫ��\n");
//		printf("100 0.0 0.3 0.7          ---------��ʾ��������0.7���µ�\n-------------------------------------\n");
//		printf("���룺");
//		scanf("%d",&n);
//		
//		j=0;
//		while(j<n){
//			scanf("%d",&vnum); /*VM��*/
//			scanf("%f%f%f",&a,&b,&htv);/*���ط�Χ�����ȶ�*/
//			/*1.�����������VM��FF���ԣ���������ģ�����ݺ���*/
//			dg.generate_data_by_vms(vnum,a,b,htv);
//	
//			printf("Total VM:%d, total PM:%d.\n",dc.sd->vm_total,dc.sd->pm_total);
//			j++;
//		}
//		break;
//	case 4:/*�������*/
//		printf("����������1��������VM��PM�������ݣ���2����PM���غ����ӿ���PM��\n");
//		printf("(1)������һ�����ݹ������N,��ʾҪ���ɼ������͵����ݵ�ͬһ���ļ���\n");
//		printf("ÿһ�ֹ�����4��������1��VM���������ã�,2��3�����ڸ�����VM������ϵ����Χ������ֹ��\n(��0 0.2����ʾ��0��0.2֮��),4��������ȶ�ϵ��(0~1����ʾPM�¶�Խ��).\n");
//	
//		printf("������ʽ�磺\n");
//		printf("1                       ----------��ʾ1�ֹ��\n");
//		printf("100 0.0 0.1 0.2          ---------��ʾ��һ����,0.2�������\n");
//		//printf("30 0.5 0.8 0.9          ----------��ʾ�ڶ�����\n");
//		//printf("100 0.0 0.3 0.7          ---------��ʾ��������\n");
//		printf("-------------------------------------\n���룺");
//		
//		scanf("%d",&n);
//		j=0;
//		while(j<n){
//			scanf("%d",&vnum); /*VM��*/
//			scanf("%f%f%f",&a,&b,&htv);/*���ط�Χ�����ȶ�*/
//			/*1.�����������VM��FF���ԣ���������ģ�����ݺ���*/
//			dg.generate_data_by_vms(vnum,a,b,htv);
//	
//			printf("Total VM:%d, total PM:%d.\n",dc.sd->vm_total,dc.sd->pm_total);
//			j++;
//		}
//	
//		break;
//	case 0:
//		return;
//		break;
//	}
//
//
//}
/*���ܣ������׺�������
@�����ߣ��½��� 2013-08-30*/
void Generate_AFF()
{
	int choice,sel,afnum;
	float a,b;
	cls_st_datagenerator dg;
	st_vm2vm vv;

	do{
		/*3.�׺���*/
		printf("����ѡ��\n--------------------------------\n");
		printf(" -  1 -  �ֹ�����VMID�����׺���.\n");
		printf(" -  2 -  �����ȡVMID�����׺���.\n");
		printf(" -  3 -  ��ʾ�����׺�����Ϣ.\n");
		printf(" -  0 -  �������˵�.\n����:");
		scanf("%d",&sel);
		if(sel==0) break;/*����*/
		switch(sel)
		{
		case 1:/*�ֹ�����VMID�����׺���*/
			
			Generate_Manul_AFF();
			
			break;
		case 2:/*����Զ������׺���*/
			Generate_AutoRND_AFF();
			break;
		case 3:/*��ʾ�����׺���*/
			showaffinitylist();
			break;
		}

	} while(1);	 


}

/*����Զ������׺���*/
void Generate_AutoRND_AFF()
{
	int afnum,choice,sel;
	float a,b;
	cls_st_datagenerator dg;

	do{
		
		printf(" - -3 -  �����׺ͣ��ڲ�ͬ�������ѡȡ����PM�������ϵ�VM֮�䴴����\n");
		printf(" - -2 -  �����׺ͣ��ڲ�ͬ�������ѡȡ����PM�������ϵ�VM֮�䴴����\n");
		printf(" - -1 -  �����׺ͣ����ѡȡ����PM�������ϵ�VM֮�䴴����\n");
		printf(" -  0 -  �����׺ͣ����ѡȡPM�������ϵ�VM֮�䴴����\n");
		printf(" -  1 -  ͬ���׺ͣ����ѡȡһ��PM�������ϵ�VM֮�䴴����\n");
		printf(" -  2 -  ͬ���׺ͣ����ѡһ���򣬿������ѡȡ����PM�������ϵ�VM֮�䴴����\n");
		printf(" -  3 -  ͬ���׺ͣ����ѡȡһ���飬�������ѡȡ����PM�������ϵ�VM֮�䴴����\n");
		printf("---------------------------------------------------------\n");
		printf(" -  6 -  ����.\n");
	
		printf("---------------------------------------------------------\n");
		printf("����:");
	
		scanf("%d",&choice);
	
		
		init_generator(&dg);
		dg.sd=&g_sharedata;

		if(choice==4)/*�ֹ����������׺���*/
		{
				
			
		}
		if(choice==6)
		{
			break;
		}
		if(choice==0)/*�����׺���*/
		{
			do {
				printf("ѡ��:\n");
				printf(" -1- ����������ɶ��������׺ͱ�.\n");
				printf(" -2- ��������VMID���������׺ͱ�.\n");
				printf(" -0- ����.");
				scanf("%d",&sel);
				
				if(sel==0) break;
				if(sel==2){
					vmPtr pvm[30];/*����30��ָ��VM��ָ��*/
					vmPtr pv;
					int vid=0;
					printf("����VMID�����ID�ÿո�ֿ�,-1Ϊ����.\n");
					printf("����:");
					do {
						scanf("%d",&vid);
						pv=&g_sharedata.vmlist[vid];
						pvm[vid]=pv;
						vid++;
					}while(vid>0);

					//pv1=pvm[0];
					//pv2=pvm[1];

				}
				if(sel==1)
				{
					printf("�������ɣ�Ҫ��������������(����n��������С��Χ����a,ֹb).\n");
					printf("  ��n a b:��ʾ����n���ߣ�������a,b֮��.\n");
					printf("  ʾ����10 0 200 ��ʾ10���ߣ�����t��ΧΪ0<t<200.\n");
					printf("==========================\n");
					printf("���룺");
					scanf("%d%f%f",&afnum, &a,&b);
					dg.create_v2v_by_ta(a,b,afnum,choice);
				}
			
			}while(sel>0);

			printf("�ɹ�����%d�������׺ͱߣ��ۼƱ�������%d.\n",
				afnum,_gthis->sd->v2v_total);
			printf("---------------------------------------------------------\n");
		}
		else/*�����׺���*/
		{	
			printf("�������׺ͱߵ�����,��10����ʾ����10���׺ͱߡ�\n���룺");
			scanf("%d",&afnum);

			dg.create_v2v_by_ta(0,0,afnum,choice);
			
			printf("�ɹ�����%d���ߣ��ۼ��׺ͱ�������%d.\n",
				afnum,_gthis->sd->v2v_total);

			printf("���룺\n");
		}
	}while(1);

}


/*����PM���˽ṹ����*/
void Generate_TOPO()
{
	int emptypmnum,sel;
	cls_st_datagenerator dg;
	
	printf("������������˽ṹ���ݣ��������˿���Ϊ24��PM��4�����ڣ�\n��������������ˣ�1-�ǣ�0-���ԣ�\n���룺");
	scanf("%d",&sel);
	if(sel==1)
	{
		printf("���������PM��:(4K��Ϊ4096)");
		scanf("%d",&emptypmnum);
	
		init_generator(&dg);
		dg.sd=&g_sharedata;

		dg.create_p2p_tree_topo(0,1,emptypmnum);/*����4K��PM����������*/

		printf("�ۼ����ɽ�����������%d,\n0��������뽻������ϵ����:%d,\n������֮�����ӹ�ϵ������%d\n",
				dg.sd->sw_total,dg.sd->p2s_total,dg.sd->s2s_total);	
	}

}


/*�ֹ�����VMID�����׺���*/
void Generate_Manul_AFF()
{
	int tot;
	st_vm2vm vv;
	int x,i=0,count,vid,fix;
	vmPtr pvm;
	char vmids[200],t_sids[200];
	int ret_i_ids[200];/*���ڷ�������������*/


	/**************************************************************/
	/*-----1.ָ��λ�õ��׺�������------------------*/
	printf("------------------------------\n");
	printf("�Ƿ�ָ�������ڰ塢����飿");
	printf("1-ָ���� 2-ָ���� 3-ָ���飬0-���ԡ�\n");
	printf("------------------------------\n");
	printf("���룺");
	scanf("%d",&fix);

	if(fix==0)/*���塢����*/
		goto bbb;
	else
	{
		vv.affinity=fix;
		if(fix==1)
			printf("������һ��ָ���������PM�����룺");
		else if(fix==2)
			printf("������һ��ָ���Ŀ�(FRAME)�����룺");
		else if(fix==3)
			printf("������һ��ָ����(GROUP)�����룺");

		scanf("%d",&vv.i_at); 
		sprintf(vv.at,"");
		if(fix==1)
			sprintf(vv.at,"%s",g_sharedata.pmlist[vv.i_at].host.sid);
		else if(fix==2)
			sprintf(vv.at,"%s",g_sharedata.framelist[vv.i_at].sid);
		else if(fix==3)
			sprintf(vv.at,"%s",g_sharedata.pmglist[vv.i_at].sid);

		getchar();

		printf("����Ҫ���õ�VMIDS(������:\"1-3 4\" ��ʾ\"1 2 3 4\"�ĸ�VM).\n���룺");
		
		//scanf("%s",vmids);
		
		gets(vmids);
		strcpy(t_sids,vmids);
		count=c_sids_to_iids(t_sids,ret_i_ids); /*��������ID����*/

		/*��IDS��Ӧ��VM����VV*/
		for(i=0;i<count;i++)
		{
			vid=ret_i_ids[i];
			vv.p_vms[i]=&g_sharedata.vmlist[vid];
			if(i==0) 
				sprintf(vv.vmids,"%s",vv.p_vms[0]->host.sid);
			else
				sprintf(vv.vmids,",%s",vv.vmids,vv.p_vms[i]->host.sid);
		}
		vv.countofvm=count;
		
		goto out_end;
	}

	/*---------------------------------------------------*/
	/*-----2.��ָ�����׺�������-----------------------------*/
bbb:
	printf("\nѡ���׺��ԣ�\n");	
	printf("----------------------------------------\n");
	printf(" - -3 -  �����׺�\n");
	printf(" - -2 -  �����׺�\n");
	printf(" - -1 -  �����׺�\n");
	printf(" -  0 -  �����׺�\n");
	printf(" -  1 -  ͬ���׺�\n");
	printf(" -  2 -  ͬ���׺�\n");
	printf(" -  3 -  ͬ���׺�\n----------------\n");
	printf(" -  4 -  ����\n");
	printf("---------------------------------------------------------\n");
	
	printf("���룺");
	scanf("%d",&vv.affinity); 		
	
	if(vv.affinity==4) return;

	if(vv.affinity<=0)/*��������ͬ�壬��ͬ�򣬲�ͬ��*/
	{
		int id1,id2;
		if(vv.affinity==0)/*�������׺�*/
		{
			printf("��������(>0��ʵ��)��");
			scanf("%lf",&vv.traffic);
		}

		printf("��������VM��ID��������VM֮���%d�׺���,\n");
		printf("��ʽ��1 2 ��ʾ����Ϊ1������Ϊ2��VM�׺�.\n",vv.affinity);
		printf("����:");
		scanf("%d%d",&id1,&id2);
		vv.p_src_vm=&g_sharedata.vmlist[id1];
		vv.p_dst_vm=&g_sharedata.vmlist[id2];
		vv.p_vms[0]=vv.p_src_vm;
		vv.p_vms[1]=vv.p_dst_vm;
		vv.countofvm=2;
		
		sprintf(vv.vmids,"%s,%s",vv.p_src_vm->host.sid,vv.p_dst_vm->host.sid);
	}
	else /*ͬ�塢ͬ�顢ͬ��*/
	{
		printf("����VM�Ķ��ID����-1�������ÿո�ֿ���\n");
		printf("�磺1 2 3 4 5 -1����ʾ����ID Ϊ1 2 3 4 5��5��VM�����׺�.\n");
		printf("----------------------------------------\n");
		printf("���룺");
		
		vv.countofvm=0;
		
		while(1){
			scanf("%d",x); 
			if(x==-1) break;
			pvm=&g_sharedata.vmlist[x];
			if(pvm->host.id>=0){
				/*��Ҫ�жϸ�VMID�����µı��*/

				if(vv.countofvm==0)
					sprintf(vv.vmids,"%s",pvm->host.sid);
				else
					sprintf(vv.vmids,"%s,%s",vv.vmids,pvm->host.sid);
				vv.p_vms[vv.countofvm++]=pvm;
			}
		};

		/**/

	}
	

out_end:
	/*���뵽V2Vlistĩβ*/
	tot=g_sharedata.v2v_total;
	vv.id=tot;
	g_sharedata.v2vlist[tot]=vv;
	g_sharedata.v2v_total++;

}
//
////���ܣ�����ģ������
//
//void GenerateData(char *RootDir)
//{
//	char url[200];
//	
//	/*����ģ������*/
//	init_drscontrol(&dc);
//	
//	/*��ʼ����������ָ��*/
//	dc.sd=&g_sharedata;
//	/*1.��������ģ�����ݵĺ�����������������global��*/
//	dc.generate_all_data();
//	
//	/*2.�����ݱ��浽XML�ļ�*/
//	/*Generate XML2*/
//	strcpy(url,RootDir);
//	strcat(url,"xml2test.xml");
//	dc.constructXML2(url);
//	
//	/*Generate XML21*/
//	strcpy(url,RootDir);
//	strcat(url,"xml21test.xml");
//	dc.constructXML21(url);
//	
//	/*Generate XML22*/
//	strcpy(url,RootDir);
//	strcat(url,"xml22test.xml");
//	dc.constructXML22(url);
//	
//	/*Output pmlist!*/
//	if(DEBUG) show_pmlist();
//	/*Output vmlist*/
//	if(DEBUG) show_vmlist();
//}
//



//���ܣ�ִ�е����㷨
//������xml1-�������� xml2-������Ϣ�ļ���xml21-����������Ϣ�ļ���xml22-�����֮���������ϵ��Ϣ��
//  xml3-���Ⱦ��߽���ļ���    
int execute(char *p_xml1, char *p_xml2, char *p_xml21, char *p_xml22, char *p_xml3)
{
	cls_st_drscontrol dc;


	return 1;
}





/* ����ע�� */
/*
 * @fn void make_whole_scheduling()
 * ִ��������ȵĲ���
 * @param host either ip address or hostname of the server
 * @param port the port number
 * @return the socket file descriptor if sucessfully, otherwise -1
 * @created by tuantuan at 2011.7.01
 * @modified by tuantuan at 2011.7.19
 */

void make_whole_scheduling()
{

	

}



/*ִ�е���ϵͳ*/
void do_schedule(char *xmlinputdir,char *outputdir)
{
	char *p_xml1="xml1.xml", *p_xml2="xml2.xml", 
		*p_xml21="xml21.xml", *p_xml22="xml22.xml", 
		*p_xml3="xml3.xml";
	char url[100];
	clock_t tpstart, tpend;
	
	cls_st_drscontrol dc;

	/*��ʼ���������ݽṹ*/
	init_shared_data();

	/*��ʼ�����ȿ�����*/
	init_drscontrol(&dc);

	dc.sd=&g_sharedata;

	tpstart=clock();
	/*�����ⲿ�����ļ�*/
	strcpy(url,xmlinputdir);
	strcat(url,p_xml1);
	dc.parseXML1(url);
	if(DEBUG) printf("Load xml1 sucessfully!\n");
	
	/*���õ�������*/
	dc.setDefaultRequest();

	/*���������ļ�*/
	strcpy(url,xmlinputdir);
	strcat(url,p_xml2);
	dc.parseXML2(url);
	if (dc.sd->readFailed){
		return ;
	}
	if(DEBUG) printf("Load xml2 sucessfully!\n");
	
	/*�������������ļ�*/
	strcpy(url,xmlinputdir);
	strcat(url,p_xml21);
	dc.parseXML21(url);
	if(DEBUG) printf("Load xml21 sucessfully!\n");
	
	/*�����׺�����������ϵ�ļ�*/
	strcpy(url,xmlinputdir);
	strcat(url,p_xml22);
	dc.parseXML22(url);

	if(DEBUG) printf("Load xml22 sucessfully!\n");
	tpend=clock();

	readxml_time=(tpend-tpstart)*1.0/CLOCKS_PER_SEC;

	/*���ݶ�ȡp_xml1��ȷ������ʲô�㷨*/

	/*����ϵͳ����״̬*/
	/*
	*  ������ڳ�ʼ���õ�VM���򴥷���ʼ���õ��ȣ�
	*  
	*  ��������ȵ㣬�򴥷��ȵ�������
	*  ���������㣬�򴥷����������
	*  ������ؾ��⣬�򴥷����ؾ��⣻
	*/

	tpstart = clock();
	
	/*���þ����㷨*/
	dc.execute_sched(actionstatus);/**/
	
	tpend=clock();
	
	dosched_time=(tpend-tpstart)*1.0/CLOCKS_PER_SEC;

	if(DEBUG) printf("\nResult:\n----------------------------------------------------------------\nScheduling is over!\n");
	/*��������Ϣ������ļ�*/
	strcpy(url,outputdir);
	strcat(url,p_xml3);
	dc.constructXML3(url);
	


	/*��ʱ����������Ϣ������ļ�*/
	strcpy(url,outputdir);
	strcat(url,"new_xml2.xml");
	dc.constructXML2(url);
	


	if(DEBUG) printf("\nSchedule and output result sucessfully! \n\nSee result in %s.\n",url);
}


/*��ʾ���е�PM����Ϣ*/
void showframelist()
{
	int i,tot;
	framePtr pfr;
	tot=g_sharedata.fr_total; 
	printf("---------------------------------------------------------\n");
	printf(" id    sid   count     pmids\n");
	printf("---------------------------------------------------------\n");
	for(i=0;i<tot;i++)
	{
		pfr=&g_sharedata.framelist[i];
		printf("%3d %6s %4d      {%s}\n",pfr->id,pfr->sid,pfr->countofpm,pfr->pmids);
	}
	printf("---------------------------------------------------------\n");
}

/*��ʾ���е�PM����Ϣ*/
void showpmglist()
{
	int i,tot;
	pmgPtr pgr;
	tot=g_sharedata.gr_total; 
	printf("---------------------------------------------------------\n");
	printf(" id    sid   count     pmids\n");
	printf("---------------------------------------------------------\n");
	for(i=0;i<tot;i++)
	{
		pgr=&g_sharedata.pmglist[i];
		//printf("%3d %6s %4d      {%s}\n",pgr->id,pgr->sid,pgr->countofpm,pgr->pmids);
	}
	printf("---------------------------------------------------------\n");
}


/*��ʾ���е�v2v�׺�����Ϣ*/
void showaffinitylist()
{
	int i,tot;
	char *nameofaff;
	vm2vmPtr pvv;
	tot=g_sharedata.v2v_total; 
	printf("---------------------------------------------------------\n");
	printf(" TYPE   TRAFFIC DELAY   AT     VMIDS\n");
	printf("---------------------------------------------------------\n");
	for(i=0;i<tot;i++)	
	{
		if(pvv->affinity==0)
			nameofaff="����";
		else if(pvv->affinity==1)
			nameofaff="ͬ��";
		else if(pvv->affinity==2)
			nameofaff="ͬ��";
		else if(pvv->affinity==3)
			nameofaff="ͬ��";
		else if(pvv->affinity==-1)
			nameofaff="��ͬ��";
		else if(pvv->affinity==-2)
			nameofaff="��ͬ��";
		else if(pvv->affinity==-3)
			nameofaff="��ͬ��";

		pvv=&g_sharedata.v2vlist[i];
		printf("%s[%2d] %6.1f %4s      {%s}\n",nameofaff,pvv->affinity,pvv->traffic,
			pvv->delay,pvv->at, pvv->vmids);
	}
	printf("---------------------------------------------------------\n");
}

