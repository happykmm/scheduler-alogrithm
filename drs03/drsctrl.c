
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

	memset(g_vmlist, 0, sizeof(g_vmlist));//初始化
	memset(g_pmlist, 0, sizeof(g_pmlist));//初始化
	memset(g_pmglist, 0, sizeof(g_pmglist));//初始化物理机组
	memset(g_framelist, 0, sizeof(g_framelist));//初始化框
	
	/*拓扑结构用 07.28 CJH ADD*/
	memset(g_swlist, 0, sizeof(g_swlist));//初始化g_swlist
	memset(g_s2slist, 0, sizeof(g_s2slist));//初始化g_s2slist
	memset(g_p2slist, 0, sizeof(g_p2slist));//初始化g_p2slist
	
	memset(g_v2vlist, 0, sizeof(g_v2vlist));//初始化虚拟机流量关系
	memset(g_p2plist, 0, sizeof(g_p2plist));//初始化物理机路径距离关系
	memset(g_v2plist, 0, sizeof(g_v2plist));//初始化虚拟机与物理机的部署信息
	memset(g_vmglist, 0, sizeof(g_vmglist));//初始化虚拟机分组
	memset(g_actionlist, 0, sizeof(g_actionlist));//初始化
	memset(g_reqlist, 0, sizeof(g_reqlist));//初始化
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
	
	dc.setDefaultRequest();


	/*3.---读入VM负载XML文件*/
	/*调用XML2文件解析函数，结果保存在pmlist\vmlist*/
	printf("\nRead xml2.xml:\n-------------------------\n");
	strcpy(url_input_file,rootpath);
	strcat(url_input_file,"xml2.xml");
	dc.parseXML2(url_input_file);
	if (dc.sd->readFailed == 1){
		return ;
	}
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


/*查询功能*/
int searchdata()
{
	int seltp=1,selt;
	int i,j;
	pmPtr ppm;
	framePtr pfr;
	pmgPtr ppmg;
	vmPtr pvm;

	while(seltp>0){
		printf("输入对象类型：");
		printf("\n--------------------------------------------------------\n");
		printf("\n0-返回\n1-PM\n2-VM\n3-group\n4-frame\n5-vm2vm\n6-vm2pm\n");
		printf("\n--------------------------------------------------------\n请输入:");
		scanf("%d",&seltp);
		switch(seltp){
		case 0:
			return 0;
			break;
		case 1:/*查所有PM*/
			printf("0-列出所有PM，1-输入PMid列出该PM的VM\n请输入：");

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
				printf("输入PMID：范围（0--%d）\n",g_sharedata.pm_total);
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
		case 2:/*查所有VM*/
			printf("0-列出所有VM，1-输入VMid列出该VM的信息.\n请输入：");
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
				printf("输入一个VMID：范围（0--%d）\n",g_sharedata.vm_total);
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
		case 3:/*查PM group*/
			printf("0-列出所有PM GROUP，1-输入PMGid列出该PMG的PM\n请输入：");

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
				printf("输入PMGID：范围（0--%d）\n",g_sharedata.gr_total);
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
		case 4:/*查PM frame*/
			printf("0-列出所有PM frame，1-输入frameid列出该frame的PM\n请输入：");

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
				printf("输入FRAMEID：范围（0--%d）\n",g_sharedata.fr_total);
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
		case 5:/*查所有的亲和性*/
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



/*功能：生成模拟数据 7.25 陈建海提交*/
void GenerateData(char *rootdir)
{
	/*初始化全局输入数据结构*/
	cls_st_datagenerator dg;
	cls_st_drscontrol dc;
	char url[200];
	int vnum,pnum,grnum,afnum,emptypmnum,n;
	float a,b,htv;
	int choice,j,sel;
	int from;

	srand((unsigned)time(NULL)); 

	if(g_sharedata.pm_total>0){
		printf("数据不为空，是否覆盖重新生成？\n 选择：-1- 覆盖; -0- 继续追加生成.\n 输入：");
		scanf("%d",&sel); 
		/*初始化*/
		if(sel==1) init_shared_data();	
		
	}else
	{
		init_shared_data();
	}
	
	/*生成模拟数据*/
	init_drscontrol(&dc);
	/*初始化共享数据指针*/
	dc.sd=&g_sharedata;
	/*1.加载生成模拟数据的函数，产生的数据在global中*/
	
	init_generator(&dg);
	dg.sd=&g_sharedata;
	
	/*初始化异构物理机规格*/
	dg.createPMV();
	
	printf("Now we generate simulation data:\n");
	
	sel=1;
	while(sel>0){
		printf("请选择：\n");
		printf("----------------------------------------------------\n");
		printf(" -1- 生成VM与PM数据；\n");
											/*1.随机生成VM与PM；
											2.向指定PM添加VM；
											3.添加空载PM；
											4.置所有PM为空载（针对初始放置数据）
											*/
		printf(" -2- 生成框数据；\n");
										/*1.基于给定PM自动生成框；
										2.添加给定数的空PM框；
										3.向给定框添加PM；
										4.给定框修改PM；*/
		printf(" -3- 生成组数据；\n");
										/*1.基于给定PM自动生成组；
										2.添加给定数的空PM组；
										3.向给定PM组添加PM；
										4.给定PM组修改PM；*/
		printf(" -4- 生成亲和性数据；\n");
		
		printf(" -5- 生成物理拓扑数据; \n");

		printf(" -6- 生成XML数据.\n "); /*1.生成XML2; 
										  2.生成XML21. 
										  3.生成XML22.
										  0.返回*/
		printf("-0- 返回.\n");
		printf("----------------------------------------------------\n");
		printf("输入：");

		scanf("%d",&sel);

		switch(sel){
		case 1:
			/*1.生成PM与VM数据*/
			Generate_PMVM();
			break;
		case 2:
			/*2.生成框*/
			Generate_FRAME();
			break;
		case 3:
			/*3.生成组*/
			Generate_PMG();
			break;
		case 4:
			/*4.生成亲和性*/
			Generate_AFF();
			break;
		case 5:/*-5- 生成物理拓扑数据;*/
			Generate_TOPO();
			break;
		case 6:/*1.生成XML2;  2.生成XML21. 3.生成XML22.*/
			Generate_XML(rootdir);
			break;
		}
	}

	printf("模拟数据生成完毕！（0-返回）\n输入:\n");
}


/*将数据保存到XML文件,导出到XML*/
void Generate_XML(char *rootdir)
{	int sel;
	char url[200];
	
	cls_st_drscontrol dc;
	init_drscontrol(&dc);
	dc.sd=&g_sharedata;

	printf("选择：\n -1- 生成XML2; \n");
	printf(" -2- 生成XML21.\n"); 
	printf(" -3- 生成XML22.\n");
	printf(" 请输入：\n");
	scanf("%d",&sel);
	
	if(sel==1){/*输出到XML2*/
		strcpy(url,rootdir);
		strcat(url,"xml2.xml");
		dc.constructXML2(url);
		printf("XML2[VM-PM]数据创建成功，请查看XML文件：%s！\n",url);
	}
	else if(sel==2){
		/*Generate XML22*/
		strcpy(url,rootdir);
		strcat(url,"xml21.xml");
		dc.constructXML21(url);
		printf("PM拓扑数据创建成功，请查看目录%s中的XML文件！\n",url);
	}
	else if(sel==3)
	{
		/*Generate XML22*/
		strcpy(url,rootdir);
		strcat(url,"xml22.xml");
		dc.constructXML22(url);
		printf("亲和性关系数据创建成功，请查看目录%s中的XML文件！\n",url);
	}
}

/*生成组*/
/*
1.基于给定PM自动生成组；
2.添加给定数的空PM组；
3.向给定PM组添加PM；
4.给定PM组修改PM；*/
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
		printf("选择：\n");
		printf(" -1- 基于所有PM，给出每个PM组的PM台数，自动随机生成PM组.\n");
		printf(" -2- 生成连续PM编号的PM组.\n");
		printf(" -3- 添加给定数的空PM组.\n");
		printf(" -4- 基于给定的多个PMID，生成一个新PM组.\n");
		printf(" -5- 更新给定PM组的PMID信息（输入给定的多个PMID，\n");
		printf("     如1-5，8,9,表示用pm1-pm5,pm8,pm9物理机更新一个PM组).\n");
		printf(" -6- 显示所有PM组列表.\n");
		printf(" -0- 返回.\n");
		
		printf("===============================\n");
		printf("输入:");
		scanf("%d",&choice);
		if(choice==0) break;
	
		switch(choice){
		case 1:/*-1- 基于所有PM，给出每个PM组的PM台数，自动随机生成PM组.\n*/
			printf("待实现......\n");
			break;
		case 2:/*生成连续PMID的PM组*/
			printf("输入：PM组个数,PM起始ID号，连续编号的PM数，格式如:\n");
			printf("---------------------------------------------------------\n");
			printf(" 3          表示要创建3个组.\n");
			printf(" 0 5        第1组：PMID从 0开始，连续5个PM创建一个PM组\n");
			printf(" 5 5        第2组：PMID从 5开始，连续5个PM创建一个PM组\n");
			printf("10 5        第3组：PMID从10开始，连续5个PM创建一个PM组\n");
			printf("---------------------------------------------------------\n");
			printf("输入:");
			scanf("%d",&grnum);
			for(j=0;j<grnum;j++){
				scanf("%d%d",&from,&pnum);
				init_generator(&dg);
				dg.create_group(from,pnum);
			}
			break;
		case 3:/*添加给定数的空PM组.*/
			printf("请输入PM组个数：");
			scanf("%d",&grnum);
			for(i=0;i<grnum;i++){
				p_pmg=&sd->pmglist[dg.sd->gr_total];
				p_pmg->countofpm=0;
				p_pmg->id=dg.sd->gr_total;
				sprintf(p_pmg->sid,"pmg%d",p_pmg->id);
				dg.sd->gr_total++;
			}
			break;
		case 4:/* -4- 基于给定的多个PMID，生成一个新PM组.*/
			printf("输入PM的多个ID：以-1结束，用空格分开。\n");
			printf("如：1 2 3 4 5 -1，表示输入ID 为1 2 3 4 5的5个PM构成一个PM组.\n");
			printf("----------------------------------------\n");
			printf("输入：");
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
					/*需要判断该pmID在留下的表格*/
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
			/*加入到pmglist*/
			dg.sd->gr_total++;

			//printf("待实现......\n");
			break;
		case 5:/*更新给定PM组的PMID信息（输入给定的多个PMID，*/
			printf("待实现......\n");
			break;
		case 6:
			showpmglist();
			//printf("待实现......\n");
			break;
		}
		printf("====================================\n");
		printf("成功创建组，累计生成Total groups:%d:.\n",dg.sd->gr_total);
	}

}


/*生成框*/
void Generate_FRAME()
{
	int choice=1,pnum;
	cls_st_datagenerator dg;
	init_generator(&dg);
	dg.sd=&g_sharedata;

	while(choice>0){
		printf("选择：\n -1- 根据给定每个框PM台数，自动生成框信息.\n");
		printf(" -2- 给定多个PMID，动态生成一个新框.\n");
		printf(" -3- 修改给定框（输入给定的多个PMID，如1-5 8 9,\n");
		printf("     表示用pm1-pm5,pm8,pm9物理机更新一个框).\n");
		printf(" -4- 显示所有框列表.\n");
		printf(" -0- 返回.\n");
		
		printf("===============================\n");
		printf("输入:");
		scanf("%d",&choice);
		if(choice==0) break;

		switch(choice){
		case 1:
			printf("输入一个框的PM台数):");
			scanf("%d",&pnum);
			dg.create_frame(pnum);
			break;
		case 2:
			printf("待实现......\n");
			break;
		case 3:
			printf("待实现......\n");
			break;
		case 4:
			showframelist();
			/*printf("待实现......\n");*/
			break;
		}

		printf("成功创建框，累计生成Total Frame:%d:.\n",dg.sd->fr_total);
		printf("---------------------------------------------------------\n");
	}
}

/*置PM为空载
printf("选择:\n -1- 置所有PM为空载.\n");
printf("-2- 输入框ID，置该框内的PM为空载.\n");
printf("-3- 输入组ID，置该组内的PM为空载.\n");
printf("-4- 输入PMID，置该PM为空载.");
printf("-0- 返回.");*/
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
	case 1:/*置所有PM为空载*/
		pmlist=g_sharedata.pmlist;
		dg.set_empty_pm(pmlist,g_sharedata.pm_total);
		printf("成功更新%d台PM为空。\n",g_sharedata.pm_total);
		break;
	case 2:
		printf("输入框ID：");
		scanf("%d",&frid);
	
		/*遍历所有PM，获取框ID对应的所有PM到lstpm数组*/
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
		printf("输入组ID：");
		scanf("%d",&grid);
	
		/*遍历所有PM，获取框ID对应的所有PM到lstpm数组*/
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

		//printf("-3- 输入组ID，置该组内的PM为空载.\n");
		//printf("待实现......");
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

	printf("是否增加空载PM?（输入：1-增加，0-忽略， 回车）\n输入：");
	/*清空PM*/
	scanf("%d",&sel);
	while(sel>0)
	{
		switch(sel)
		{
		case 1:
			printf("请输入空载PM数（N）（建议与前面累计生成的PM数一起凑成一个整数，如82，+新输入118=200）.\n输入：");
			scanf("%d",&pnum); /*台数*/
			dg.create_empty_pm(pnum);
			printf("累计total PM:%d.\n",dc.sd->pm_total);
			
			break;
		case 2:
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
		}
		
		printf("是否继续增加空载PM?（1-增加，0-忽略，回车）\n输入：");
		scanf("%d",&sel);
	}
		
}


/*生成PMVM*/
/*1.随机生成VM与PM；
2.向指定PM添加VM；
3.添加空载PM；
4.置所有PM为空载（针对初始放置数据）
*/
void Generate_PMVM()
{
	int i,j,n,vnum,pnum,sel=1,opt;
	float a,b,htv;
	cls_st_datagenerator dg;
	init_generator(&dg);
	dg.sd=&g_sharedata;

	while(sel>0){
		printf("请选择：\n -1- 随机生成VM与PM.\n");
		printf(" -2- 向指定PM添加VM.\n");
		printf(" -3- 添加空载PM.\n");
		printf(" -4- 置PM为空载（针对初始放置数据）.\n");
		printf(" -0- 返回.\n");
		printf("输入:");
	
		scanf("%d",&sel);
		if(sel==0) break;

		switch(sel){
		case 1:
			printf("--------------------------------------------------------\n");
			printf("输入：\n 数据规格数N,表示要生成N种规格的数据。\n");
			printf(" 每种规格的4个参数：\n VM台数（表示待放置或要生成的VM数）\n ");
			printf(" 两个0到1的数（表示VM缩放系数范围）（起、止）(如0 0.2，表示在0到0.2之间),\n");
			printf(" 物理机温度(0~1，表示用VM装PM后将达到的温度).\n");
	
			printf("输入格式如下：\n");
			printf("3                       ----------表示3种规格\n");
			printf("100 0.0 0.1 0.2          ---------表示第一组规格\n");
			printf("30 0.5 0.8 0.9          ----------表示第二组规格\n");
			printf("100 0.0 0.3 0.7          ---------表示第三组规格\n");
			printf("-------------------------------------\n");
			printf("输入：");
			scanf("%d",&n);
		
			j=0;
			while(j<n){
				scanf("%d",&vnum);
				scanf("%f%f%f",&a,&b,&htv);/*负载范围及冷热度*/
				/*1.生成物理机与VM，FF策略，加载生成模拟数据函数*/
				dg.generate_data_by_vms(vnum,a,b,htv);
			
				printf("累计生成Total VM:%d, total PM:%d.\n",dg.sd->vm_total,dg.sd->pm_total);
				j++;
			}

			break;
		case 2:/* -2- 向指定PM添加VM.*/
			/*选择输入指定PM的内码，列出该PM内的所有VM，
			再输入要加入新开的VM台数，*/
			{
				int pn,vn;
				scanf("%d%d",&pn,&vn);/*输入PM台数,VM台数*/
				//scanf();
			}
			break;
		case 3:
			//Generate_EPTPM();/*添加空载PM*/
			printf("输入:要生成的空载PM数（N）（建议与前面累计生成的PM数一起凑成一个整数，\n");
			printf(" 如82，+新输入118=200）.\n输入：");
			scanf("%d",&pnum); /*台数*/
			dg.create_empty_pm(pnum);
			printf("累计total PM:%d.\n",dg.sd->pm_total);
			
			break;
		case 4:
			/*清空PM数据*/
			opt=1;
			while(opt>0){
				printf("选择:\n -1- 置所有PM为空载.\n");
				printf("-2- 输入框ID，置该框内的PM为空载.\n");
				printf("-3- 输入组ID，置该组内的PM为空载.\n");
				printf("-4- 输入PMID，置该PM为空载.\n");
				printf("-0- 返回.\n 输入:");
				
				scanf("%d",&opt);
				if(opt==0) break;
				if(opt==1)
					SetEMPTYPM(opt);
			}
			break;
		}

	}

}



//	printf("1.生成PM与VM数据:要求输入VM数（待放置），PM数（包括空载），VM缩放系数，冷热度\n");
//	
//	/*支持多场景创建*/
//	printf("请选择调度决策类型：\n -1- 初始放置；\n -2- 动态调度：负载均衡；\n");
//	/*printf("   3-动态调度-热点消除;\n   4-动态调度-冷点整合.\n   0-退出。\n-----------------\n输入：");*/
//
//	printf("   3-动态调度-热点消除;\n   4-动态调度-冷点整合.\n   0-退出。\n-----------------\n输入：");
//	
//	Generate_EPTPM();
//
//	scanf("%d",&sel);
//	switch (sel)
//	{
//	case 1:/*初始放置数据:包括各种冷热度，<0.2,<0.7,<0.9*/
//			
//		//scanf("%f%f%f",&a,&b,&htv);
//		
//
//		break;
//	case 2:/*负载均衡*/
//		printf("分两步：（1）生成全数据；（2）置PM空载和增加空载PM。\n");
//		printf("(1)先输入一个数据规格种数N,表示要生成几种类型的数据到同一个文件。\n");
//		printf("每一种规格包括4个参数：1）VM数（待放置）,2、3）VM缩放系数范围：（起、止）(如0 0.2，表示在0到0.2之间),\n4）物理机热度系数(0~1，表示PM温度越大).\n");
//	
//		printf("输入样式如：\n");
//		printf("3                       ----------表示3种规格\n");
//		printf("100 0.0 0.1 0.2         ----------表示第一组规格\n");
//		printf("30 0.5 0.8 0.9          ----------表示第二组规格\n");
//		printf("100 0.0 0.3 0.7         ----------表示第三组规格\n------------------------------------\n");
//		printf("输入：");
//		
//		scanf("%d",&n);
//		j=0;
//		while(j<n){
//			scanf("%d",&vnum); /*VM数*/
//			scanf("%f%f%f",&a,&b,&htv);/*负载范围及冷热度*/
//			/*1.生成物理机与VM，FF策略，加载生成模拟数据函数*/
//			dg.generate_data_by_vms(vnum,a,b,htv);
//	
//			printf("Total VM:%d, total PM:%d.\n",dc.sd->vm_total,dc.sd->pm_total);
//			j++;
//		}
//
//
//		break;
//	case 3:/*热点消除*/
//		
//		printf("分两步：（1）生成全数据；（2）置PM空载和增加空载PM。\n");
//		printf("(1)先输入一个数据规格种数N,表示要生成几种类型的数据到同一个文件。\n");
//		printf("每一种规格包括4个参数：1）VM数（待放置）,2、3）VM缩放系数范围：（起、止）(如0 0.2，表示在0到0.2之间),\n4）物理机热度系数(0~1，表示PM温度越大).\n");
//	
//		printf("输入样式如：\n");
//		printf("3                        ----------表示3种规格\n");
//		printf("100 0.0 0.1 0.2          ---------表示第一组规格：0.2是冷点\n");
//		printf("30 0.5 0.8 0.9           ----------表示第二组规格：0.9是热点必须要有\n");
//		printf("100 0.0 0.3 0.7          ---------表示第三组规格：0.7是温点\n-------------------------------------\n");
//		printf("输入：");
//		scanf("%d",&n);
//		
//		j=0;
//		while(j<n){
//			scanf("%d",&vnum); /*VM数*/
//			scanf("%f%f%f",&a,&b,&htv);/*负载范围及冷热度*/
//			/*1.生成物理机与VM，FF策略，加载生成模拟数据函数*/
//			dg.generate_data_by_vms(vnum,a,b,htv);
//	
//			printf("Total VM:%d, total PM:%d.\n",dc.sd->vm_total,dc.sd->pm_total);
//			j++;
//		}
//		break;
//	case 4:/*冷点整合*/
//		printf("分两步：（1）生基本VM、PM负载数据；（2）置PM空载和增加空载PM。\n");
//		printf("(1)先输入一个数据规格种数N,表示要生成几种类型的数据到同一个文件。\n");
//		printf("每一种规格包括4个参数：1）VM数（待放置）,2、3）基于给定的VM的缩放系数范围：（起、止）\n(如0 0.2，表示在0到0.2之间),4）物理机热度系数(0~1，表示PM温度越大).\n");
//	
//		printf("输入样式如：\n");
//		printf("1                       ----------表示1种规格\n");
//		printf("100 0.0 0.1 0.2          ---------表示第一组规格,0.2冷点数据\n");
//		//printf("30 0.5 0.8 0.9          ----------表示第二组规格\n");
//		//printf("100 0.0 0.3 0.7          ---------表示第三组规格\n");
//		printf("-------------------------------------\n输入：");
//		
//		scanf("%d",&n);
//		j=0;
//		while(j<n){
//			scanf("%d",&vnum); /*VM数*/
//			scanf("%f%f%f",&a,&b,&htv);/*负载范围及冷热度*/
//			/*1.生成物理机与VM，FF策略，加载生成模拟数据函数*/
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
/*功能：生成亲和性数据
@创建者：陈建海 2013-08-30*/
void Generate_AFF()
{
	int choice,sel,afnum;
	float a,b;
	cls_st_datagenerator dg;
	st_vm2vm vv;

	do{
		/*3.亲和性*/
		printf("操作选择：\n--------------------------------\n");
		printf(" -  1 -  手工输入VMID生成亲和性.\n");
		printf(" -  2 -  随机获取VMID生成亲和性.\n");
		printf(" -  3 -  显示所有亲和性信息.\n");
		printf(" -  0 -  返回主菜单.\n输入:");
		scanf("%d",&sel);
		if(sel==0) break;/*返回*/
		switch(sel)
		{
		case 1:/*手工输入VMID生成亲和性*/
			
			Generate_Manul_AFF();
			
			break;
		case 2:/*随机自动产生亲和性*/
			Generate_AutoRND_AFF();
			break;
		case 3:/*显示所有亲和性*/
			showaffinitylist();
			break;
		}

	} while(1);	 


}

/*随机自动产生亲和性*/
void Generate_AutoRND_AFF()
{
	int afnum,choice,sel;
	float a,b;
	cls_st_datagenerator dg;

	do{
		
		printf(" - -3 -  反组亲和：在不同组中随机选取两个PM，在其上的VM之间创建；\n");
		printf(" - -2 -  反框亲和：在不同框中随机选取两个PM，在其上的VM之间创建；\n");
		printf(" - -1 -  反板亲和：随机选取两个PM，在其上的VM之间创建；\n");
		printf(" -  0 -  流量亲和：随机选取PM，在其上的VM之间创建；\n");
		printf(" -  1 -  同板亲和：随机选取一个PM，在其上的VM之间创建；\n");
		printf(" -  2 -  同框亲和：随机选一个框，框中随机选取两个PM，在其上的VM之间创建；\n");
		printf(" -  3 -  同组亲和：随机选取一个组，组中随机选取两个PM，在其上的VM之间创建；\n");
		printf("---------------------------------------------------------\n");
		printf(" -  6 -  返回.\n");
	
		printf("---------------------------------------------------------\n");
		printf("输入:");
	
		scanf("%d",&choice);
	
		
		init_generator(&dg);
		dg.sd=&g_sharedata;

		if(choice==4)/*手工输入生成亲和性*/
		{
				
			
		}
		if(choice==6)
		{
			break;
		}
		if(choice==0)/*流量亲和性*/
		{
			do {
				printf("选择:\n");
				printf(" -1- 随机批量生成多条流量亲和边.\n");
				printf(" -2- 输入两个VMID生成流量亲和边.\n");
				printf(" -0- 返回.");
				scanf("%d",&sel);
				
				if(sel==0) break;
				if(sel==2){
					vmPtr pvm[30];/*定义30个指向VM的指针*/
					vmPtr pv;
					int vid=0;
					printf("输入VMID：多个ID用空格分开,-1为结束.\n");
					printf("输入:");
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
					printf("批量生成，要求输入三个数：(边数n，流量大小范围（起a,止b).\n");
					printf("  如n a b:表示生成n条边，流量在a,b之间.\n");
					printf("  示例：10 0 200 表示10条边，流量t范围为0<t<200.\n");
					printf("==========================\n");
					printf("输入：");
					scanf("%d%f%f",&afnum, &a,&b);
					dg.create_v2v_by_ta(a,b,afnum,choice);
				}
			
			}while(sel>0);

			printf("成功生成%d条流量亲和边！累计边总数：%d.\n",
				afnum,_gthis->sd->v2v_total);
			printf("---------------------------------------------------------\n");
		}
		else/*其它亲和性*/
		{	
			printf("请输入亲和边的数量,如10，表示创建10条亲和边。\n输入：");
			scanf("%d",&afnum);

			dg.create_v2v_by_ta(0,0,afnum,choice);
			
			printf("成功生成%d条边！累计亲和边总数：%d.\n",
				afnum,_gthis->sd->v2v_total);

			printf("输入：\n");
		}
	}while(1);

}


/*生成PM拓扑结构数据*/
void Generate_TOPO()
{
	int emptypmnum,sel;
	cls_st_datagenerator dg;
	
	printf("创建物理机拓扑结构数据？交换机端口数为24，PM有4块网口，\n随机生成树形拓扑（1-是，0-忽略）\n输入：");
	scanf("%d",&sel);
	if(sel==1)
	{
		printf("输入物理机PM数:(4K即为4096)");
		scanf("%d",&emptypmnum);
	
		init_generator(&dg);
		dg.sd=&g_sharedata;

		dg.create_p2p_tree_topo(0,1,emptypmnum);/*生成4K个PM的网络拓扑*/

		printf("累计生成交换机总数：%d,\n0层物理机与交换机关系总数:%d,\n交换机之间连接关系总数：%d\n",
				dg.sd->sw_total,dg.sd->p2s_total,dg.sd->s2s_total);	
	}

}


/*手工输入VMID生成亲和性*/
void Generate_Manul_AFF()
{
	int tot;
	st_vm2vm vv;
	int x,i=0,count,vid,fix;
	vmPtr pvm;
	char vmids[200],t_sids[200];
	int ret_i_ids[200];/*用于返回整数的数组*/


	/**************************************************************/
	/*-----1.指定位置的亲和性设置------------------*/
	printf("------------------------------\n");
	printf("是否指定放置在板、框或组？");
	printf("1-指定板 2-指定框 3-指定组，0-忽略。\n");
	printf("------------------------------\n");
	printf("输入：");
	scanf("%d",&fix);

	if(fix==0)/*定板、框、组*/
		goto bbb;
	else
	{
		vv.affinity=fix;
		if(fix==1)
			printf("请输入一个指定的物理机PM的内码：");
		else if(fix==2)
			printf("请输入一个指定的框(FRAME)的内码：");
		else if(fix==3)
			printf("请输入一个指定组(GROUP)的内码：");

		scanf("%d",&vv.i_at); 
		sprintf(vv.at,"");
		if(fix==1)
			sprintf(vv.at,"%s",g_sharedata.pmlist[vv.i_at].host.sid);
		else if(fix==2)
			sprintf(vv.at,"%s",g_sharedata.framelist[vv.i_at].sid);
		else if(fix==3)
			sprintf(vv.at,"%s",g_sharedata.pmglist[vv.i_at].sid);

		getchar();

		printf("输入要放置的VMIDS(按规则:\"1-3 4\" 表示\"1 2 3 4\"四个VM).\n输入：");
		
		//scanf("%s",vmids);
		
		gets(vmids);
		strcpy(t_sids,vmids);
		count=c_sids_to_iids(t_sids,ret_i_ids); /*获得输入的ID数组*/

		/*将IDS对应的VM生成VV*/
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
	/*-----2.非指定的亲和性设置-----------------------------*/
bbb:
	printf("\n选择亲和性：\n");	
	printf("----------------------------------------\n");
	printf(" - -3 -  反组亲和\n");
	printf(" - -2 -  反框亲和\n");
	printf(" - -1 -  反板亲和\n");
	printf(" -  0 -  流量亲和\n");
	printf(" -  1 -  同板亲和\n");
	printf(" -  2 -  同框亲和\n");
	printf(" -  3 -  同组亲和\n----------------\n");
	printf(" -  4 -  返回\n");
	printf("---------------------------------------------------------\n");
	
	printf("输入：");
	scanf("%d",&vv.affinity); 		
	
	if(vv.affinity==4) return;

	if(vv.affinity<=0)/*流量，不同板，不同框，不同组*/
	{
		int id1,id2;
		if(vv.affinity==0)/*是流量亲和*/
		{
			printf("输入流量(>0的实数)：");
			scanf("%lf",&vv.traffic);
		}

		printf("输入两个VM的ID生成两个VM之间的%d亲和性,\n");
		printf("格式：1 2 表示内码为1与内码为2的VM亲和.\n",vv.affinity);
		printf("输入:");
		scanf("%d%d",&id1,&id2);
		vv.p_src_vm=&g_sharedata.vmlist[id1];
		vv.p_dst_vm=&g_sharedata.vmlist[id2];
		vv.p_vms[0]=vv.p_src_vm;
		vv.p_vms[1]=vv.p_dst_vm;
		vv.countofvm=2;
		
		sprintf(vv.vmids,"%s,%s",vv.p_src_vm->host.sid,vv.p_dst_vm->host.sid);
	}
	else /*同板、同组、同框*/
	{
		printf("输入VM的多个ID：以-1结束，用空格分开。\n");
		printf("如：1 2 3 4 5 -1，表示输入ID 为1 2 3 4 5的5个VM构成亲和.\n");
		printf("----------------------------------------\n");
		printf("输入：");
		
		vv.countofvm=0;
		
		while(1){
			scanf("%d",x); 
			if(x==-1) break;
			pvm=&g_sharedata.vmlist[x];
			if(pvm->host.id>=0){
				/*需要判断该VMID在留下的表格*/

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
	/*加入到V2Vlist末尾*/
	tot=g_sharedata.v2v_total;
	vv.id=tot;
	g_sharedata.v2vlist[tot]=vv;
	g_sharedata.v2v_total++;

}
//
////功能：生成模拟数据
//
//void GenerateData(char *RootDir)
//{
//	char url[200];
//	
//	/*生成模拟数据*/
//	init_drscontrol(&dc);
//	
//	/*初始化共享数据指针*/
//	dc.sd=&g_sharedata;
//	/*1.加载生成模拟数据的函数，产生的数据在global中*/
//	dc.generate_all_data();
//	
//	/*2.将数据保存到XML文件*/
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



//功能：执行调度算法
//参数：xml1-调度请求； xml2-负载信息文件；xml21-物理拓扑信息文件；xml22-虚拟机之间的流量关系信息；
//  xml3-调度决策结果文件；    
int execute(char *p_xml1, char *p_xml2, char *p_xml21, char *p_xml22, char *p_xml3)
{
	cls_st_drscontrol dc;


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

	

}



/*执行调度系统*/
void do_schedule(char *xmlinputdir,char *outputdir)
{
	char *p_xml1="xml1.xml", *p_xml2="xml2.xml", 
		*p_xml21="xml21.xml", *p_xml22="xml22.xml", 
		*p_xml3="xml3.xml";
	char url[100];
	clock_t tpstart, tpend;
	
	cls_st_drscontrol dc;

	/*初始化共享数据结构*/
	init_shared_data();

	/*初始化调度控制器*/
	init_drscontrol(&dc);

	dc.sd=&g_sharedata;

	tpstart=clock();
	/*解析外部请求文件*/
	strcpy(url,xmlinputdir);
	strcat(url,p_xml1);
	dc.parseXML1(url);
	if(DEBUG) printf("Load xml1 sucessfully!\n");
	
	/*设置调度请求*/
	dc.setDefaultRequest();

	/*解析负载文件*/
	strcpy(url,xmlinputdir);
	strcat(url,p_xml2);
	dc.parseXML2(url);
	if (dc.sd->readFailed){
		return ;
	}
	if(DEBUG) printf("Load xml2 sucessfully!\n");
	
	/*解析物理拓扑文件*/
	strcpy(url,xmlinputdir);
	strcat(url,p_xml21);
	dc.parseXML21(url);
	if(DEBUG) printf("Load xml21 sucessfully!\n");
	
	/*解析亲和性与流量关系文件*/
	strcpy(url,xmlinputdir);
	strcat(url,p_xml22);
	dc.parseXML22(url);

	if(DEBUG) printf("Load xml22 sucessfully!\n");
	tpend=clock();

	readxml_time=(tpend-tpstart)*1.0/CLOCKS_PER_SEC;

	/*根据读取p_xml1后，确定调用什么算法*/

	/*分析系统负载状态*/
	/*
	*  如果存在初始放置的VM，则触发初始放置调度；
	*  
	*  如果存在热点，则触发热点消除；
	*  如果存在冷点，则触发冷点消除；
	*  如果负载均衡，则触发负载均衡；
	*/

	tpstart = clock();
	
	/*调用具体算法*/
	dc.execute_sched(actionstatus);/**/
	
	tpend=clock();
	
	dosched_time=(tpend-tpstart)*1.0/CLOCKS_PER_SEC;

	if(DEBUG) printf("\nResult:\n----------------------------------------------------------------\nScheduling is over!\n");
	/*输出结果信息到结果文件*/
	strcpy(url,outputdir);
	strcat(url,p_xml3);
	dc.constructXML3(url);
	


	/*临时：输出结果信息到结果文件*/
	strcpy(url,outputdir);
	strcat(url,"new_xml2.xml");
	dc.constructXML2(url);
	


	if(DEBUG) printf("\nSchedule and output result sucessfully! \n\nSee result in %s.\n",url);
}


/*显示所有的PM组信息*/
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

/*显示所有的PM组信息*/
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


/*显示所有的v2v亲和性信息*/
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
			nameofaff="流量";
		else if(pvv->affinity==1)
			nameofaff="同板";
		else if(pvv->affinity==2)
			nameofaff="同框";
		else if(pvv->affinity==3)
			nameofaff="同组";
		else if(pvv->affinity==-1)
			nameofaff="不同板";
		else if(pvv->affinity==-2)
			nameofaff="不同框";
		else if(pvv->affinity==-3)
			nameofaff="不同组";

		pvv=&g_sharedata.v2vlist[i];
		printf("%s[%2d] %6.1f %4s      {%s}\n",nameofaff,pvv->affinity,pvv->traffic,
			pvv->delay,pvv->at, pvv->vmids);
	}
	printf("---------------------------------------------------------\n");
}

