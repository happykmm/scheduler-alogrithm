
/* 文件注释 */
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

/*-------数据处理类变量声明------------------------------------*/
cls_st_xmlprocess xmlread;/*xml数据读取类变量，负责读取解析外部XML到相应数据结构*/

/*--------调度算法类变量声明--------------------*/
cls_st_vm_group vmgrp; /*分组类*/

cls_st_drscontrol dc; /*调度控制类*/

/**************************************************************/
/*Declations of global functions*/

/*init_shared_data for this system*/
void init_shared_data();

/*加载XML2,XML21,XML22*/
void loaddata(char *rootpath);

/*生成模拟数据函数*/
void GenerateData(char *RootDir);

/*执行调度决策*/
void do_schedule(char *xmlinputdir,char *outputdir);


int execute(char *p_xml1, char *p_xml2, char *p_xml21, char *p_xml22, char *p_xml3);


/*按照调度选择进行整体调度测试*/
void make_whole_scheduling();
void GenerateData1(char *rootdir);

/*---------------------------------------------------------------*/
/*****************************************************************/
/*格式：drsmain "xml1" "xml2" "xml21" "xml22" "xmlresult")*/
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

	char xmldir[100]="./input/test01/";/*文件所在目录*/
	/*亲和性数据*/
	char xmlaffdir[100]="./input/Data/affinity/";
	/*负载均衡*/
	char xmlaffdir1[100]="./input/Data/hotsolve/";

	char xmlnewfiledir[100]="./input/Data/xmltest/";/*新产生的模拟数据所在目录*/

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
		
			/*初始化全局输入数据结构*/
			init_shared_data();	
			/*从文件加载所有数据*/
			
			//loaddata(xmldir);
			printf("加载数据测试:\n1.加载%s;\n2.加载%s.\n3.加载%s.\n4.加载%s\n5.加载%s\n0-结束.\n输入：",
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
	
				printf("加载数据测试:\n1.加载%s;\n2.加载%s.\n3.加载%s.\n4.加载%s\n5.加载%s\n0-结束.\n输入：",
					xmlaffdir,xmlaffdir1,xmldir,xmlnewfiledir,xmlaffdir2);
				scanf("%d",&sel);
			}

			break;
		
		case 2:
			/*初始化全局输入数据结构*/
			//init_shared_data();	
			/*调用产生数据模块，生成xml模拟数据文件到xmlnewfiledir目录下*/
			 //GenerateData(xmlnewfiledir);	
			printf("模拟数据生成，输出见目录%s............\n",xmlnewfiledir);
			printf("分四步完成：\n1.生成VM和PM(见XML2)；\n");
			printf("2.生成框(见XML2)；\n3.生成组(见XML2)；");
			printf("\n4.生成亲和关系(见XML22)；\n输入:1-开始，0-结束！\n");

			scanf("%d",&sel);
			while(sel==1){
				GenerateData1(xmlnewfiledir);
				scanf("%d",&sel);
			}
			printf("Please find and check result file in dir: %s.\n",xmlnewfiledir);
			break;	
		case 3:/*由华为提供*/
			
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
		
		case 5: /*通用调度函数*/
			
			/*初始化全局输入数据结构*/
			init_shared_data();	
			/*A whole scheduling test.*/
			execute(xml[0],xml[1],xml[2],xml[3],xml[4]);
			break;
		case 6:
			//生成XML3
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
	memset(g_vmlist, 0, sizeof(g_vmlist));//初始化
	memset(g_pmlist, 0, sizeof(g_pmlist));//初始化
	memset(g_pmglist, 0, sizeof(g_pmglist));//初始化物理机组
	memset(g_framelist, 0, sizeof(g_framelist));//初始化框
	
	/*拓扑结构用 07.28 CJH ADD*/
	memset(g_swlist, 0, sizeof(g_swlist));//初始化g_swlist
	memset(g_s2slist, 0, sizeof(g_s2slist));//初始化g_s2slist
	memset(g_p2slist, 0, sizeof(g_p2slist));//初始化g_p2slist
	
	memset(g_v2vlist, 0, sizeof(g_v2vlist));//初始化虚拟机流量关系
	memset(g_afflist, 0, sizeof(g_afflist));//初始化亲和性关系
	memset(g_p2plist, 0, sizeof(g_p2plist));//初始化物理机路径距离关系
	memset(g_v2plist, 0, sizeof(g_v2plist));//初始化虚拟机与物理机的部署信息
	memset(g_vmglist, 0, sizeof(g_vmglist));//初始化虚拟机分组
	memset(g_actionlist, 0, sizeof(g_actionlist));//初始化
	memset(g_reqlist, 0, sizeof(g_reqlist));//初始化
	
	
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
	

/*加载XML1, XML2,XML21,XML22*/
void loaddata(char *rootpath)
{
	char url_input_file[200];
	printf("Xml datas locate in %s:\n",rootpath);
				
	/*1.---初始化控制器*/
	init_drscontrol(&dc);
	/*初始化共享数据指针*/
	dc.sd=&g_sharedata;
	
	/*2.---调用XML1文件解析函数，结果保存在requestlist*/
	//读入控制请求信息文件
	g_sharedata.req_total=0;
	printf("\nRead xml1.xml:\n-------------------------\n");
	strcpy(url_input_file,rootpath);
	strcat(url_input_file,"xml1.xml");
	dc.parseXML1(url_input_file);
	printf("---END XML1----------------\n\n");
	
	/*3.---读入VM负载XML文件*/
	/*调用XML2文件解析函数，结果保存在pmlist\vmlist*/
	printf("\nRead xml2.xml:\n-------------------------\n");
	strcpy(url_input_file,rootpath);
	strcat(url_input_file,"xml2.xml");
	dc.parseXML2(url_input_file);
	/*读入*/
	if(DEBUG) printf("\n\nPM总数：%d，VM总数：%d，框的个数：%d,组的个数：%d",
		dc.sd->pm_total,dc.sd->vm_total,dc.sd->fr_total,dc.sd->gr_total);
	
	printf("\n---END XML2----------------\n\n");

	/*/读入PM2PM之间的时延关系文件*/
	printf("\nRead xml21.xml:\n-------------------------\n"); 
	strcpy(url_input_file,rootpath);
	strcat(url_input_file,"xml21.xml");
	dc.parseXML21(url_input_file);
	//xmlread.get_pm2pm_from_xml21(url_input_file);
	printf("---END XML21----------------\n\n");
	
	
	/*读入VM之间的连接关系文件*/
	printf("Read xml22.xml:\n-------------------------\n");
	strcpy(url_input_file,rootpath);
	strcat(url_input_file,"xml22.xml");
	dc.parseXML22(url_input_file);
	printf("---END XML22----------------\n\n");
	


	printf("Load data from %s sucessfully!\n",rootpath);
}



/*功能：生成模拟数据 7.25 陈建海提交*/
void GenerateData1(char *rootdir)
{
	/*初始化全局输入数据结构*/
	cls_st_datagenerator dg;
	cls_st_drscontrol dc;
	char url[200];
	int vnum,pnum,grnum,afnum,emptypmnum,n;
	float a,b,htv;
	int choice,j,sel;

	srand((unsigned)time(NULL)); 

	/*初始化*/
	init_shared_data();	

	/*生成模拟数据*/
	init_drscontrol(&dc);
	
	/*初始化共享数据指针*/
	dc.sd=&g_sharedata;
	/*1.加载生成模拟数据的函数，产生的数据在global中*/
	
	init_generator(&dg);
	dg.sd=&g_sharedata;
	
	/*初始化异构物理机规格*/
	dg.createPMV();
	
	/**/
	printf("Now we generate simulation data:\n");
	

	printf("输入：1.生成PM与VM数据；2-物理拓扑数据。\n");
	
	scanf("%d",&sel);
	if(sel==2) goto topo;


	printf("1.生成PM与VM数据:要求输入VM数（待放置），PM数（包括空载），VM缩放系数，冷热度\n");
	
	/*支持多场景创建*/
	printf("请选择调度决策类型生成相应数据：\n   1-初始放置；\n   2-动态调度：负载均衡；\n");
	printf("   3-动态调度-热点消除;\n   4-动态调度-冷点整合.\n   0-退出。\n-----------------\n输入：");
	
	scanf("%d",&sel);
	
	switch (sel)
	{
	case 1:/*初始放置数据:包括各种冷热度，<0.2,<0.7,<0.9*/
			
		//scanf("%f%f%f",&a,&b,&htv);
		printf("分两步：（1）生成全数据；（2）置PM空载和增加空载PM。\n");
		printf("(1)先输入一个数据规格种数N,表示要生成几种类型的数据到同一个文件。\n");
		printf("每一种规格包括4个参数：1）VM数（待放置）,2、3）VM缩放系数范围：（起、止）(如0 0.2，表示在0到0.2之间),\n4）物理机热度系数(0~1，表示PM温度越大).\n");
	
		printf("输入样式如：\n");
		
		printf("3                       ----------表示3种规格\n");
		printf("100 0.0 0.1 0.2          ---------表示第一组规格\n");
		printf("30 0.5 0.8 0.9          ----------表示第二组规格\n");
		printf("100 0.0 0.3 0.7          ---------表示第三组规格\n-------------------------------------\n");
		printf("输入：");
		scanf("%d",&n);
		
		j=0;
		while(j<n){
			scanf("%d",&vnum);
			scanf("%f%f%f",&a,&b,&htv);/*负载范围及冷热度*/
			/*1.生成物理机与VM，FF策略，加载生成模拟数据函数*/
			dg.generate_data_by_vms(vnum,a,b,htv);
			
			printf("累计生成Total VM:%d, total PM:%d.\n",dc.sd->vm_total,dc.sd->pm_total);
			j++;
		}

		
		break;
	case 2:/*负载均衡*/
		printf("分两步：（1）生成全数据；（2）置PM空载和增加空载PM。\n");
		printf("(1)先输入一个数据规格种数N,表示要生成几种类型的数据到同一个文件。\n");
		printf("每一种规格包括4个参数：1）VM数（待放置）,2、3）VM缩放系数范围：（起、止）(如0 0.2，表示在0到0.2之间),\n4）物理机热度系数(0~1，表示PM温度越大).\n");
	
		printf("输入样式如：\n");
		printf("3                       ----------表示3种规格\n");
		printf("100 0.0 0.1 0.2         ----------表示第一组规格\n");
		printf("30 0.5 0.8 0.9          ----------表示第二组规格\n");
		printf("100 0.0 0.3 0.7         ----------表示第三组规格\n------------------------------------\n");
		printf("输入：");
		
		scanf("%d",&n);
		j=0;
		while(j<n){
			scanf("%d",&vnum); /*VM数*/
			scanf("%f%f%f",&a,&b,&htv);/*负载范围及冷热度*/
			/*1.生成物理机与VM，FF策略，加载生成模拟数据函数*/
			dg.generate_data_by_vms(vnum,a,b,htv);
	
			printf("Total VM:%d, total PM:%d.\n",dc.sd->vm_total,dc.sd->pm_total);
			j++;
		}


		break;
	case 3:/*热点消除*/
		
		printf("分两步：（1）生成全数据；（2）置PM空载和增加空载PM。\n");
		printf("(1)先输入一个数据规格种数N,表示要生成几种类型的数据到同一个文件。\n");
		printf("每一种规格包括4个参数：1）VM数（待放置）,2、3）VM缩放系数范围：（起、止）(如0 0.2，表示在0到0.2之间),\n4）物理机热度系数(0~1，表示PM温度越大).\n");
	
		printf("输入样式如：\n");
		printf("3                        ----------表示3种规格\n");
		printf("100 0.0 0.1 0.2          ---------表示第一组规格：0.2是冷点\n");
		printf("30 0.5 0.8 0.9           ----------表示第二组规格：0.9是热点必须要有\n");
		printf("100 0.0 0.3 0.7          ---------表示第三组规格：0.7是温点\n-------------------------------------\n");
		printf("输入：");
		scanf("%d",&n);
		
		j=0;
		while(j<n){
			scanf("%d",&vnum); /*VM数*/
			scanf("%f%f%f",&a,&b,&htv);/*负载范围及冷热度*/
			/*1.生成物理机与VM，FF策略，加载生成模拟数据函数*/
			dg.generate_data_by_vms(vnum,a,b,htv);
	
			printf("Total VM:%d, total PM:%d.\n",dc.sd->vm_total,dc.sd->pm_total);
			j++;
		}


		break;
	case 4:/*冷点整合*/
		printf("分两步：（1）生成全数据；（2）置PM空载和增加空载PM。\n");
		printf("(1)先输入一个数据规格种数N,表示要生成几种类型的数据到同一个文件。\n");
		printf("每一种规格包括4个参数：1）VM数（待放置）,2、3）VM缩放系数范围：（起、止）(如0 0.2，表示在0到0.2之间),\n4）物理机热度系数(0~1，表示PM温度越大).\n");
	
		printf("输入样式如：\n");
		printf("1                       ----------表示1种规格\n");
		printf("100 0.0 0.1 0.2          ---------表示第一组规格,0.2冷点数据\n");
		//printf("30 0.5 0.8 0.9          ----------表示第二组规格\n");
		//printf("100 0.0 0.3 0.7          ---------表示第三组规格\n");
		printf("-------------------------------------\n输入：");
		
		
		scanf("%d",&n);
		j=0;
		while(j<n){
			scanf("%d",&vnum); /*VM数*/
			scanf("%f%f%f",&a,&b,&htv);/*负载范围及冷热度*/
			/*1.生成物理机与VM，FF策略，加载生成模拟数据函数*/
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
	printf("请选择调度决策类型：\n1-初始放置；\n2-动态调度：负载均衡；\n");
	printf("3-动态调度-热点消除;\n4-动态调度-冷点整合.\n0-退出。\n");
	scanf("%d",&sel);
	*/

	printf("是否增加空载PM?（输入：1-增加，0-忽略， 回车）\n输入：");
	/*清空PM*/
	scanf("%d",&sel);
	if(sel==1)
	{
		printf("请输入空载PM数（N）（建议与前面累计生成的PM数一起凑成一个整数，如82，+新输入118=200）.\n输入：");
		scanf("%d",&pnum);
		dg.create_empty_pm(pnum);
		printf("累计total PM:%d.\n",dc.sd->pm_total);
	}

	
	/*2.生成框和组*/
	
	printf("2.生成框吗？每个框PM台数自动生成框的信息(输入：0-跳过,1-继续)\n输入:");
	scanf("%d",&choice);
	if(choice==0) goto cgroup;

	printf("输入(PM台数):");
	scanf("%d",&pnum);
	//frnum=_gthis->sd->pm_total/vmcount_perfr+1; /*计算所需框的总数*/
	dg.create_frame(pnum);

	printf("成功创建框，累计生成Total Frame:%d:.\n",dc.sd->fr_total);


cgroup:
	
	printf("3.创建PM分组？（1-创建，0-跳过）\n输入：");
	
	scanf("%d",&choice);
	if(choice==0) goto crv2v;
	
	printf("需要分别创建组及该组的PM数，先输入组个数n,再输入每组的PM数,格式如:\n3\n 5 5 5.\n");
	
	scanf("%d",&grnum);
	
	dc.sd->gr_total=0;

	for(j=0;j<grnum;j++)
	{
		scanf("%d",&pnum);
		dg.create_group(pnum);
	}

	printf("成功创建组，累计生成Total groups:%d:.\n",dc.sd->gr_total);


	
	/*输出到XML2*/
	/*2.将数据保存到XML文件*/
	/*Generate XML2*/
	strcpy(url,rootdir);
	strcat(url,"xml2.xml");
	dc.constructXML2(url);
	
	printf("VMPM模拟数据创建成功，请查看XML文件：%s！\n",url);


crv2v:	
	/*3.亲和性*/
	
	printf("4.创建亲和性关系，请选择类型：\n--------------------------------\n");
	printf("-3：-反组亲和：在不同组中随机选取两个PM，在其上的VM之间创建；\n");
	printf("-2：-反框亲和：在不同框中随机选取两个PM，在其上的VM之间创建；\n");
	printf("-1：-反板亲和：随机选取两个PM，在其上的VM之间创建；\n");
	printf(" 0：-流量亲和：随机选取PM，在其上的VM之间创建；\n");
	printf(" 1：-同板亲和：随机选取一个PM，在其上的VM之间创建；\n");
	printf(" 2：-同框亲和：随机选一个框，框中随机选取两个PM，在其上的VM之间创建；\n");
	printf(" 3：-同组亲和：随机选取一个组，组中随机选取两个PM，在其上的VM之间创建；\n");
	printf("---------------------------------------------------------\n");
	printf("请输入类型号:(4-退出)：");
	
	scanf("%d",&choice);
	
	if(choice>3 || choice <-3) goto outp;

	init_generator(&dg);
	dg.sd=&g_sharedata;

	while(choice>=-3 && choice <=3)
	{
		
		if(choice==0)
		{
			printf("请输入边的数量，及流量大小的范围，如(n a b),\n如，生成10条边，流量0,200mbps之间,则输入10 0 200，\n输入：");
			scanf("%d%f%f",&afnum, &a,&b);
			
			dg.create_v2v_by_ta(a,b,afnum,choice);

			printf("成功生成%d条边！累计亲和边总数：%d\n输入（4-退出）：",afnum,_gthis->sd->v2v_total);
			
		}
		else
		{	
			printf("请输入数量,如10，表示创建10条亲和边。\n输入：");
			
			scanf("%d",&afnum);
			
			dg.create_v2v_by_ta(0,0,afnum,choice);
			
			printf("成功生成%d条边！累计亲和边总数：%d.\n输入（4-退出）：",afnum,_gthis->sd->v2v_total);
			
		}

		scanf("%d",&choice);
	}
	

	
	printf("是否置所有PM为空载(针对初始放置)？(输入1-是，0-忽略）\n输入：");
	scanf("%d",&sel);
	if(sel==1){
		dg.set_empty_pm();		
			/*输出到XML2*/
		/*2.将数据保存到XML文件*/
		/*Generate XML2*/
		strcpy(url,rootdir);
		strcat(url,"xml2.xml");
		dc.constructXML2(url);
	
		printf("VMPM模拟数据创建成功，请查看XML文件：%s！\n-------------------------------------\n输入(0-返回):",url);
	}
	

	/*Generate XML22*/
	strcpy(url,rootdir);
	strcat(url,"xml22.xml");
	dc.constructXML22(url);

	printf("模拟数据创建成功，请查看目录%s中的XML文件！\n",url);

topo:	
	printf("创建物理机拓扑结构数据？交换机端口数为24，PM有4块网口，\n随机生成树形拓扑（1-是，0-忽略）\n输入：");
	scanf("%d",&sel);
	if(sel==1)
	{
		printf("输入物理机PM数:(4K即为4096)");
		scanf("%d",&emptypmnum);
		dg.create_p2p_tree_topo(0,1,emptypmnum);/*生成4K个PM的网络拓扑*/
	}
	else
		return;

	printf("累计生成交换机总数：%d,\n0层物理机与交换机关系总数:%d,\n交换机之间连接关系总数：%d\n",
		_gthis->sd->sw_total,_gthis->sd->p2s_total,_gthis->sd->s2s_total);	

	
	
	/*Generate XML21*/
	strcpy(url,rootdir);
	strcat(url,"xml21.xml");
	dc.constructXML21(url);

	printf("模拟数据创建成功，请查看目录%s中的XML文件！\n",url);


outp:
	printf("模拟数据生成完毕！（0-返回）\n输入:\n");
}

//功能：生成模拟数据

void GenerateData(char *RootDir)
{
	char url[200];
	
	/*生成模拟数据*/
	init_drscontrol(&dc);
	
	/*初始化共享数据指针*/
	dc.sd=&g_sharedata;
	/*1.加载生成模拟数据的函数，产生的数据在global中*/
	dc.generate_all_data();
	
	/*2.将数据保存到XML文件*/
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




//功能：执行调度算法
//参数：xml1-调度请求； xml2-负载信息文件；xml21-物理拓扑信息文件；xml22-虚拟机之间的流量关系信息；
//  xml3-调度决策结果文件；    
int execute(char *p_xml1, char *p_xml2, char *p_xml21, char *p_xml22, char *p_xml3)
{
	cls_st_drscontrol dc;

	/*初始化共享数据结构*/
	init_shared_data();

	/*初始化调度控制器*/
	init_drscontrol(&dc);

	dc.sd=&g_sharedata;


	/*解析外部请求文件*/
	dc.parseXML1(p_xml1);

	/*解析负载文件*/
	dc.parseXML2(p_xml2);
	
	/*解析物理拓扑文件*/
	dc.parseXML21(p_xml21);
	
	/*解析亲和性与流量关系文件*/
	dc.parseXML22(p_xml22);


	/*根据读取p_xml1后，确定调用什么算法*/

	
	/*分析系统负载状态*/
	/*
	*  如果存在初始放置的VM，则触发初始放置调度；
	*  
	*  如果存在热点，则触发热点消除；
	*  如果存在冷点，则触发冷点消除；
	*  如果负载均衡，则触发负载均衡；
	*/

	/*调用具体算法*/
	dc.execute_sched(1);

	/*输出结果信息到结果文件*/
	dc.constructXML3(p_xml3);

	return 1;
}





/* 函数注释 */
/*
 * @fn void make_whole_scheduling()
 * 执行整体调度的测试
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
	/*初始化控制器*/
	init_drscontrol(&dc);
	/*初始化共享数据指针*/
	dc.sd=&g_sharedata;

	//提示决策的选择
	printf("Enter scheduling decision choice:\n");
	printf("1-Initplace scheduling.\n");
	printf("2-Load balance scheduling.\n");
	printf("3-Hotsolve scheduling.\n");
	printf("4-Coldsolve or consolidation scheduling.\n");
	printf("0-exit.\n");
	scanf("%d",&decistat);

	switch(decistat){
	case 1:
		/*初试放置*/
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