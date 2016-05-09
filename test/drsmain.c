
/* �ļ�ע�� */
/*
 * @file drsctrl.c the input of tdrs main program
 * @created  at 2013.04.10 by JianhaiChen <chenjh919@zju.edu.cn>
 * @modified by JianhaiChen at 2013.05.13
 */

/***********************************************/
#include <stdio.h>
#include <string.h>
//#include <stdlib.h>
#include <time.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

/***********************************************/
#include "lib/drsdata.h"
#include "lib/drsdef.h"
#include "lib/drsfunc.h"
#include "lib/drsglobal.h"
#include "lib/drslib.h"

/***********************************************/
#include "drscontroller.h"
#include "dataprocess/xmlprocess.h"
#include "dataprocess/generator.h"


/***********************************************/
//#include "scheduler/alg_groupcut.h"

/**************************************************************/
/*Declarations of operation class */

/*-------���ݴ������������------------------------------------*/
cls_st_xmlprocess xmlread;/*xml���ݶ�ȡ������������ȡ�����ⲿXML����Ӧ���ݽṹ*/

/*--------�����㷨���������--------------------*/
cls_st_vm_group vmgrp; /*������*/

cls_st_drscontrol dc; /*���ȿ�����*/

/**************************************************************/
/*Declations of global functions*/

/*init_shared_data for this system*/
void init_shared_data();

/*����XML2,XML21,XML22*/
void loaddata(char *rootpath);

/*����ģ�����ݺ���*/
void GenerateData(char *RootDir);

/*ִ�е��Ⱦ���*/
void do_schedule(char *xmlinputdir,char *outputdir);


int execute(char *p_xml1, char *p_xml2, char *p_xml21, char *p_xml22, char *p_xml3);


/*���յ���ѡ�����������Ȳ���*/
void make_whole_scheduling();
void GenerateData1(char *rootdir);

/*---------------------------------------------------------------*/
/*****************************************************************/
/*��ʽ��drsmain "xml1" "xml2" "xml21" "xml22" "xmlresult")*/
/*****************************************************************/


int main()//int argc, char* argv[])
{
	int choice,sel;
	//int decistat;
	char xml[5][30]={
		"xml1.xml",
		"xml2.xml",
		"xml21.xml",
		"xml22.xml",
		"xml3.xml"
	};
	//char *fpxml[5];

	char xmldir[100]="./input/test01/";/*�ļ�����Ŀ¼*/
	/*�׺�������*/
	char xmlaffdir[100]="./input/Data/affinity/";
	/*���ؾ���*/
	char xmlaffdir1[100]="./input/Data/hotsolve/";

	char xmlnewfiledir[100]="./input/Data/xmltest/";/*�²�����ģ����������Ŀ¼*/

	char xmlaffdir2[100]="./input/Data/initplace/";

	
	//char urlfile[200];

	//FILE *fp;
	
	/*
	if(argc!=6) 
	{
		printf("error, please input 5 parameters!\n");
		exit(0);
	}

	for(i=1;i<argc;i++)
	{
		if((fp=fopen(argv[i],"r"))==NULL)
		{
			printf("%s File open error, please check this file is existed!\n",argv[i]);
			exit(0);
		}
		else 
		{
			fpxml[i]=argv[i];
			fclose(fp);
		}

	}

	printf("All files are existed!\n");

	///////////////////////////////////////////////
	//exit(0);

	*/

	while(1){
		printf("Enter choice:\n");
		printf("1-Load data XML1, XML2(VM & PM), XML21(PM2PM), XML22(VM2VM) from xmldata.\n");
		printf("2-Generate XML2, XML21, XML22 data from generator and initialize all globals.\n");
		printf("3-Getdata from outer cloud computing platforms and initialize all globals.\n");
		printf("4-Make a whole scheduling test and see the result in xml3.\n");
		printf("5-Make scheduling for test, take initialplace as an example by grouping and packing.\n");
		printf("6-Output the actionlist into xml file.\n");		
		printf("0-exit.\n");
		scanf("%d",&choice);
		if(choice==0) break;
		
		switch(choice){
		case 1:
		
			/*��ʼ��ȫ���������ݽṹ*/
			init_shared_data();	
			/*���ļ�������������*/
			
			//loaddata(xmldir);
			printf("�������ݲ���:\n1.����%s;\n2.����%s.\n3.����%s.\n4.����%s\n5.����%s\n0-����.\n���룺",
					xmlaffdir,xmlaffdir1,xmldir,xmlnewfiledir,xmlaffdir2);
				
			scanf("%d",&sel);
			
			while(sel>0){
				
				switch (sel)
				{
				case 1:
					loaddata(xmlaffdir);
					break;
				case 2:

					loaddata(xmlaffdir1);
					break;
				case 3:

					loaddata(xmldir);

					break;
				case 4:
					loaddata(xmlnewfiledir);
					break;
				case 5:
					loaddata(xmlaffdir2);
					break;
				}
	
				printf("�������ݲ���:\n1.����%s;\n2.����%s.\n3.����%s.\n4.����%s\n5.����%s\n0-����.\n���룺",
					xmlaffdir,xmlaffdir1,xmldir,xmlnewfiledir,xmlaffdir2);
				scanf("%d",&sel);
			}

			break;
		
		case 2:
			/*��ʼ��ȫ���������ݽṹ*/
			//init_shared_data();	
			/*���ò�������ģ�飬����xmlģ�������ļ���xmlnewfiledirĿ¼��*/
			 //GenerateData(xmlnewfiledir);	
			printf("ģ���������ɣ������Ŀ¼%s............\n",xmlnewfiledir);
			printf("���Ĳ���ɣ�\n1.����VM��PM(��XML2)��\n");
			printf("2.���ɿ�(��XML2)��\n3.������(��XML2)��");
			printf("\n4.�����׺͹�ϵ(��XML22)��\n����:1-��ʼ��0-������\n");

			scanf("%d",&sel);
			while(sel==1){
				GenerateData1(xmlnewfiledir);
				scanf("%d",&sel);
			}
			printf("Please find and check result file in dir: %s.\n",xmlnewfiledir);
			break;	
		case 3:/*�ɻ�Ϊ�ṩ*/
			
			/**/
			//getdatafromcloud();
			{
				int *ar,i,sum=0;
				ar=(int*)malloc(10*sizeof(int));
				for(i=0;i<10;i++)
				{
					ar[i]=i;
					sum+=ar[i];
				}
				printf("%d\n",sum);
				free(ar);			
			}


			break;/**/
		
		case 4:
			
			
			make_whole_scheduling();
			

			break;
		
		case 5: /*ͨ�õ��Ⱥ���*/
			
			/*��ʼ��ȫ���������ݽṹ*/
			init_shared_data();	
			/*A whole scheduling test.*/
			execute(xml[0],xml[1],xml[2],xml[3],xml[4]);
			break;
		case 6:
			//����XML3
			{
				cls_st_xmlprocess xp;
				//g_sharedata.action_total=0;
			
				xp.sd=&g_sharedata;
				init_xmlprocess(&xp);
				xp.xml_create_by_actions("xml3-1.xml");
			}

			printf("CPU:%d\n",CPU);
			break;
		}	
	}
	
	printf("Thank you for your using!\n");
	
}


/************************************************/
/*initial the global data struc.*/
/******************************************/
void init_shared_data()
{
	memset(g_vmlist, 0, sizeof(g_vmlist));//��ʼ��
	memset(g_pmlist, 0, sizeof(g_pmlist));//��ʼ��
	memset(g_pmglist, 0, sizeof(g_pmglist));//��ʼ���������
	memset(g_framelist, 0, sizeof(g_framelist));//��ʼ����
	
	/*���˽ṹ�� 07.28 CJH ADD*/
	memset(g_swlist, 0, sizeof(g_swlist));//��ʼ��g_swlist
	memset(g_s2slist, 0, sizeof(g_s2slist));//��ʼ��g_s2slist
	memset(g_p2slist, 0, sizeof(g_p2slist));//��ʼ��g_p2slist
	
	memset(g_v2vlist, 0, sizeof(g_v2vlist));//��ʼ�������������ϵ
	memset(g_afflist, 0, sizeof(g_afflist));//��ʼ���׺��Թ�ϵ
	memset(g_p2plist, 0, sizeof(g_p2plist));//��ʼ�������·�������ϵ
	memset(g_v2plist, 0, sizeof(g_v2plist));//��ʼ���������������Ĳ�����Ϣ
	memset(g_vmglist, 0, sizeof(g_vmglist));//��ʼ�����������
	memset(g_actionlist, 0, sizeof(g_actionlist));//��ʼ��
	memset(g_reqlist, 0, sizeof(g_reqlist));//��ʼ��
	
	
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
	g_sharedata.v2v_total = 0;
	g_sharedata.afflist_total = 0;


	g_sharedata.reqlist=g_reqlist;
	g_sharedata.vmlist=g_vmlist;
	g_sharedata.pmlist=g_pmlist;
	g_sharedata.pmglist=g_pmglist;
	g_sharedata.framelist=g_framelist;
	g_sharedata.v2vlist=g_v2vlist;
	g_sharedata.afflist = g_afflist;
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
	
	/*3.---����VM����XML�ļ�*/
	/*����XML2�ļ��������������������pmlist\vmlist*/
	printf("\nRead xml2.xml:\n-------------------------\n");
	strcpy(url_input_file,rootpath);
	strcat(url_input_file,"xml2.xml");
	dc.parseXML2(url_input_file);
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



/*���ܣ�����ģ������ 7.25 �½����ύ*/
void GenerateData1(char *rootdir)
{
	/*��ʼ��ȫ���������ݽṹ*/
	cls_st_datagenerator dg;
	cls_st_drscontrol dc;
	char url[200];
	int vnum,pnum,grnum,afnum,emptypmnum,n;
	float a,b,htv;
	int choice,j,sel;

	srand((unsigned)time(NULL)); 

	/*��ʼ��*/
	init_shared_data();	

	/*����ģ������*/
	init_drscontrol(&dc);
	
	/*��ʼ����������ָ��*/
	dc.sd=&g_sharedata;
	/*1.��������ģ�����ݵĺ�����������������global��*/
	
	init_generator(&dg);
	dg.sd=&g_sharedata;
	
	/*��ʼ���칹��������*/
	dg.createPMV();
	
	/**/
	printf("Now we generate simulation data:\n");
	

	printf("���룺1.����PM��VM���ݣ�2-�����������ݡ�\n");
	
	scanf("%d",&sel);
	if(sel==2) goto topo;


	printf("1.����PM��VM����:Ҫ������VM���������ã���PM�����������أ���VM����ϵ�������ȶ�\n");
	
	/*֧�ֶೡ������*/
	printf("��ѡ����Ⱦ�������������Ӧ���ݣ�\n   1-��ʼ���ã�\n   2-��̬���ȣ����ؾ��⣻\n");
	printf("   3-��̬����-�ȵ�����;\n   4-��̬����-�������.\n   0-�˳���\n-----------------\n���룺");
	
	scanf("%d",&sel);
	
	switch (sel)
	{
	case 1:/*��ʼ��������:�����������ȶȣ�<0.2,<0.7,<0.9*/
			
		//scanf("%f%f%f",&a,&b,&htv);
		printf("����������1������ȫ���ݣ���2����PM���غ����ӿ���PM��\n");
		printf("(1)������һ�����ݹ������N,��ʾҪ���ɼ������͵����ݵ�ͬһ���ļ���\n");
		printf("ÿһ�ֹ�����4��������1��VM���������ã�,2��3��VM����ϵ����Χ������ֹ��(��0 0.2����ʾ��0��0.2֮��),\n4��������ȶ�ϵ��(0~1����ʾPM�¶�Խ��).\n");
	
		printf("������ʽ�磺\n");
		
		printf("3                       ----------��ʾ3�ֹ��\n");
		printf("100 0.0 0.1 0.2          ---------��ʾ��һ����\n");
		printf("30 0.5 0.8 0.9          ----------��ʾ�ڶ�����\n");
		printf("100 0.0 0.3 0.7          ---------��ʾ��������\n-------------------------------------\n");
		printf("���룺");
		scanf("%d",&n);
		
		j=0;
		while(j<n){
			scanf("%d",&vnum);
			scanf("%f%f%f",&a,&b,&htv);/*���ط�Χ�����ȶ�*/
			/*1.�����������VM��FF���ԣ���������ģ�����ݺ���*/
			dg.generate_data_by_vms(vnum,a,b,htv);
			
			printf("�ۼ�����Total VM:%d, total PM:%d.\n",dc.sd->vm_total,dc.sd->pm_total);
			j++;
		}

		
		break;
	case 2:/*���ؾ���*/
		printf("����������1������ȫ���ݣ���2����PM���غ����ӿ���PM��\n");
		printf("(1)������һ�����ݹ������N,��ʾҪ���ɼ������͵����ݵ�ͬһ���ļ���\n");
		printf("ÿһ�ֹ�����4��������1��VM���������ã�,2��3��VM����ϵ����Χ������ֹ��(��0 0.2����ʾ��0��0.2֮��),\n4��������ȶ�ϵ��(0~1����ʾPM�¶�Խ��).\n");
	
		printf("������ʽ�磺\n");
		printf("3                       ----------��ʾ3�ֹ��\n");
		printf("100 0.0 0.1 0.2         ----------��ʾ��һ����\n");
		printf("30 0.5 0.8 0.9          ----------��ʾ�ڶ�����\n");
		printf("100 0.0 0.3 0.7         ----------��ʾ��������\n------------------------------------\n");
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
	case 3:/*�ȵ�����*/
		
		printf("����������1������ȫ���ݣ���2����PM���غ����ӿ���PM��\n");
		printf("(1)������һ�����ݹ������N,��ʾҪ���ɼ������͵����ݵ�ͬһ���ļ���\n");
		printf("ÿһ�ֹ�����4��������1��VM���������ã�,2��3��VM����ϵ����Χ������ֹ��(��0 0.2����ʾ��0��0.2֮��),\n4��������ȶ�ϵ��(0~1����ʾPM�¶�Խ��).\n");
	
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
	case 4:/*�������*/
		printf("����������1������ȫ���ݣ���2����PM���غ����ӿ���PM��\n");
		printf("(1)������һ�����ݹ������N,��ʾҪ���ɼ������͵����ݵ�ͬһ���ļ���\n");
		printf("ÿһ�ֹ�����4��������1��VM���������ã�,2��3��VM����ϵ����Χ������ֹ��(��0 0.2����ʾ��0��0.2֮��),\n4��������ȶ�ϵ��(0~1����ʾPM�¶�Խ��).\n");
	
		printf("������ʽ�磺\n");
		printf("1                       ----------��ʾ1�ֹ��\n");
		printf("100 0.0 0.1 0.2          ---------��ʾ��һ����,0.2�������\n");
		//printf("30 0.5 0.8 0.9          ----------��ʾ�ڶ�����\n");
		//printf("100 0.0 0.3 0.7          ---------��ʾ��������\n");
		printf("-------------------------------------\n���룺");
		
		
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
	case 0:
		return;
		break;
	}
/*
	printf("��ѡ����Ⱦ������ͣ�\n1-��ʼ���ã�\n2-��̬���ȣ����ؾ��⣻\n");
	printf("3-��̬����-�ȵ�����;\n4-��̬����-�������.\n0-�˳���\n");
	scanf("%d",&sel);
	*/

	printf("�Ƿ����ӿ���PM?�����룺1-���ӣ�0-���ԣ� �س���\n���룺");
	/*���PM*/
	scanf("%d",&sel);
	if(sel==1)
	{
		printf("���������PM����N����������ǰ���ۼ����ɵ�PM��һ��ճ�һ����������82��+������118=200��.\n���룺");
		scanf("%d",&pnum);
		dg.create_empty_pm(pnum);
		printf("�ۼ�total PM:%d.\n",dc.sd->pm_total);
	}

	
	/*2.���ɿ����*/
	
	printf("2.���ɿ���ÿ����PM̨���Զ����ɿ����Ϣ(���룺0-����,1-����)\n����:");
	scanf("%d",&choice);
	if(choice==0) goto cgroup;

	printf("����(PM̨��):");
	scanf("%d",&pnum);
	//frnum=_gthis->sd->pm_total/vmcount_perfr+1; /*��������������*/
	dg.create_frame(pnum);

	printf("�ɹ��������ۼ�����Total Frame:%d:.\n",dc.sd->fr_total);


cgroup:
	
	printf("3.����PM���飿��1-������0-������\n���룺");
	
	scanf("%d",&choice);
	if(choice==0) goto crv2v;
	
	printf("��Ҫ�ֱ𴴽��鼰�����PM���������������n,������ÿ���PM��,��ʽ��:\n3\n 5 5 5.\n");
	
	scanf("%d",&grnum);
	
	dc.sd->gr_total=0;

	for(j=0;j<grnum;j++)
	{
		scanf("%d",&pnum);
		dg.create_group(pnum);
	}

	printf("�ɹ������飬�ۼ�����Total groups:%d:.\n",dc.sd->gr_total);


	
	/*�����XML2*/
	/*2.�����ݱ��浽XML�ļ�*/
	/*Generate XML2*/
	strcpy(url,rootdir);
	strcat(url,"xml2.xml");
	dc.constructXML2(url);
	
	printf("VMPMģ�����ݴ����ɹ�����鿴XML�ļ���%s��\n",url);


crv2v:	
	/*3.�׺���*/
	
	printf("4.�����׺��Թ�ϵ����ѡ�����ͣ�\n--------------------------------\n");
	printf("-3��-�����׺ͣ��ڲ�ͬ�������ѡȡ����PM�������ϵ�VM֮�䴴����\n");
	printf("-2��-�����׺ͣ��ڲ�ͬ�������ѡȡ����PM�������ϵ�VM֮�䴴����\n");
	printf("-1��-�����׺ͣ����ѡȡ����PM�������ϵ�VM֮�䴴����\n");
	printf(" 0��-�����׺ͣ����ѡȡPM�������ϵ�VM֮�䴴����\n");
	printf(" 1��-ͬ���׺ͣ����ѡȡһ��PM�������ϵ�VM֮�䴴����\n");
	printf(" 2��-ͬ���׺ͣ����ѡһ���򣬿������ѡȡ����PM�������ϵ�VM֮�䴴����\n");
	printf(" 3��-ͬ���׺ͣ����ѡȡһ���飬�������ѡȡ����PM�������ϵ�VM֮�䴴����\n");
	printf("---------------------------------------------------------\n");
	printf("���������ͺ�:(4-�˳�)��");
	
	scanf("%d",&choice);
	
	if(choice>3 || choice <-3) goto outp;

	init_generator(&dg);
	dg.sd=&g_sharedata;

	while(choice>=-3 && choice <=3)
	{
		
		if(choice==0)
		{
			printf("������ߵ���������������С�ķ�Χ����(n a b),\n�磬����10���ߣ�����0,200mbps֮��,������10 0 200��\n���룺");
			scanf("%d%f%f",&afnum, &a,&b);
			
			dg.create_v2v_by_ta(a,b,afnum,choice);

			printf("�ɹ�����%d���ߣ��ۼ��׺ͱ�������%d\n���루4-�˳�����",afnum,_gthis->sd->v2v_total);
			
		}
		else
		{	
			printf("����������,��10����ʾ����10���׺ͱߡ�\n���룺");
			
			scanf("%d",&afnum);
			
			dg.create_v2v_by_ta(0,0,afnum,choice);
			
			printf("�ɹ�����%d���ߣ��ۼ��׺ͱ�������%d.\n���루4-�˳�����",afnum,_gthis->sd->v2v_total);
			
		}

		scanf("%d",&choice);
	}
	

	
	printf("�Ƿ�������PMΪ����(��Գ�ʼ����)��(����1-�ǣ�0-���ԣ�\n���룺");
	scanf("%d",&sel);
	if(sel==1){
		dg.set_empty_pm();		
			/*�����XML2*/
		/*2.�����ݱ��浽XML�ļ�*/
		/*Generate XML2*/
		strcpy(url,rootdir);
		strcat(url,"xml2.xml");
		dc.constructXML2(url);
	
		printf("VMPMģ�����ݴ����ɹ�����鿴XML�ļ���%s��\n-------------------------------------\n����(0-����):",url);
	}
	

	/*Generate XML22*/
	strcpy(url,rootdir);
	strcat(url,"xml22.xml");
	dc.constructXML22(url);

	printf("ģ�����ݴ����ɹ�����鿴Ŀ¼%s�е�XML�ļ���\n",url);

topo:	
	printf("������������˽ṹ���ݣ��������˿���Ϊ24��PM��4�����ڣ�\n��������������ˣ�1-�ǣ�0-���ԣ�\n���룺");
	scanf("%d",&sel);
	if(sel==1)
	{
		printf("���������PM��:(4K��Ϊ4096)");
		scanf("%d",&emptypmnum);
		dg.create_p2p_tree_topo(0,1,emptypmnum);/*����4K��PM����������*/
	}
	else
		return;

	printf("�ۼ����ɽ�����������%d,\n0��������뽻������ϵ����:%d,\n������֮�����ӹ�ϵ������%d\n",
		_gthis->sd->sw_total,_gthis->sd->p2s_total,_gthis->sd->s2s_total);	

	
	
	/*Generate XML21*/
	strcpy(url,rootdir);
	strcat(url,"xml21.xml");
	dc.constructXML21(url);

	printf("ģ�����ݴ����ɹ�����鿴Ŀ¼%s�е�XML�ļ���\n",url);


outp:
	printf("ģ������������ϣ���0-���أ�\n����:\n");
}

//���ܣ�����ģ������

void GenerateData(char *RootDir)
{
	char url[200];
	
	/*����ģ������*/
	init_drscontrol(&dc);
	
	/*��ʼ����������ָ��*/
	dc.sd=&g_sharedata;
	/*1.��������ģ�����ݵĺ�����������������global��*/
	dc.generate_all_data();
	
	/*2.�����ݱ��浽XML�ļ�*/
	/*Generate XML2*/
	strcpy(url,RootDir);
	strcat(url,"xml2test.xml");
	dc.constructXML2(url);
	
	/*Generate XML21*/
	strcpy(url,RootDir);
	strcat(url,"xml21test.xml");
	dc.constructXML21(url);
	
	/*Generate XML22*/
	strcpy(url,RootDir);
	strcat(url,"xml22test.xml");
	dc.constructXML22(url);
	
	/*Output pmlist!*/
	if(DEBUG) show_pmlist();
	/*Output vmlist*/
	if(DEBUG) show_vmlist();
}




//���ܣ�ִ�е����㷨
//������xml1-�������� xml2-������Ϣ�ļ���xml21-����������Ϣ�ļ���xml22-�����֮���������ϵ��Ϣ��
//  xml3-���Ⱦ��߽���ļ���    
int execute(char *p_xml1, char *p_xml2, char *p_xml21, char *p_xml22, char *p_xml3)
{
	cls_st_drscontrol dc;

	/*��ʼ���������ݽṹ*/
	init_shared_data();

	/*��ʼ�����ȿ�����*/
	init_drscontrol(&dc);

	dc.sd=&g_sharedata;


	/*�����ⲿ�����ļ�*/
	dc.parseXML1(p_xml1);

	/*���������ļ�*/
	dc.parseXML2(p_xml2);
	
	/*�������������ļ�*/
	dc.parseXML21(p_xml21);
	
	/*�����׺�����������ϵ�ļ�*/
	dc.parseXML22(p_xml22);


	/*���ݶ�ȡp_xml1��ȷ������ʲô�㷨*/

	
	/*����ϵͳ����״̬*/
	/*
	*  ������ڳ�ʼ���õ�VM���򴥷���ʼ���õ��ȣ�
	*  
	*  ��������ȵ㣬�򴥷��ȵ�������
	*  ���������㣬�򴥷����������
	*  ������ؾ��⣬�򴥷����ؾ��⣻
	*/

	/*���þ����㷨*/
	dc.execute_sched(1);

	/*��������Ϣ������ļ�*/
	dc.constructXML3(p_xml3);

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
	int decistat;

	g_sharedata.vmg_total=0;
	/*��ʼ��������*/
	init_drscontrol(&dc);
	/*��ʼ����������ָ��*/
	dc.sd=&g_sharedata;

	//��ʾ���ߵ�ѡ��
	printf("Enter scheduling decision choice:\n");
	printf("1-Initplace scheduling.\n");
	printf("2-Load balance scheduling.\n");
	printf("3-Hotsolve scheduling.\n");
	printf("4-Coldsolve or consolidation scheduling.\n");
	printf("0-exit.\n");
	scanf("%d",&decistat);

	switch(decistat){
	case 1:
		/*���Է���*/
		decistat=DOINITPLACE;
		dc.execute_sched(decistat);
		
		break;
	case 2:


		break;
	case 3:

		dc.execute_sched(decistat);
		break;
	case 4:

		break;
	}
	

}