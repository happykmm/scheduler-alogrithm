#include "../drsctrl.h"
#define URL_TOTAL 200
/*for testing*/
int main(){
	int choice,selurl,i;
	char xmldir[100];
	
	char schetype[4][30]={
		"initplace","loadbalance","coldsolve","hotsolve"
	};
	
	/*测试数据所在根目录*/
	char xmlrootdir[30]="./Data/input/";

	/*测试数据所在主目录*/
	char xmlaffdir[5][100]={
		"initplace/",/*初始放置*/
		"loadbalance/",/*负载均衡*/
		"coldsolve/",/*冷点整合*/
		"hotsolve/"/*热点消除*/
	};
	
	char xmlaffsubdir[URL_TOTAL][100]={
		"test01/",
		"test02/",
		"test03/",
		"test04/",
		"test05/",
		"test06/",
		"test07/",
		"test08/",
		"test09/",
		"test10/",
		"test11/",
		"test12/",
		"test13/",
		"test14/",
		"test15/",
		"test16/",
		"test17/",
		"test18/",
		"test19/",
		"test20/",
		"test21/",
		"test22/",
		"test23/",
		"test24/",
		"test25/",
		"test26/",
"test27/",
"test28/",
"test29/",
"test30/",

	};
	
	/*新产生的模拟数据所在目录*/
	char xmltestdir[100]="./Data/input/xmltest/";
	
start:
	choice=1;
	while(choice>0){
		//ScheduleActions {DOINITPLACE = 1, DOLOADBALANCE = 2, DOCONSOLIDATE = 3, DOHOTSOLVE = 4 };
		printf("********************************************************\n");
		printf("*** Wellcome to TDRS testing system version 1.0.     ***\n");
		printf("********************************************************\n\n");
		printf("* Begin testing, select a scheduling option as follows:\n");
		printf("--------------------------------------------------------\n");
		printf("-1- Initial place:(prepare source data from [%s%s])\n",xmlrootdir,xmlaffdir[0]);
		printf("-2- load balance:(prepare source data from [%s%s])\n",xmlrootdir,xmlaffdir[1]);
		printf("-3- cold solve:(prepare source data from [%s%s])\n",xmlrootdir,xmlaffdir[2]);
		printf("-4- hot solve:(prepare source data from [%s%s])\n",xmlrootdir,xmlaffdir[3]);
		printf("--------------------------------------------------------\n");
		printf("*-for data generation:\n");
		printf("--------------------------------------------------------\n");
		printf("-5- Generate simulation data: data in [%s]\n",xmltestdir);
		printf("-6- Load simulation data: data in [%s]\n",xmltestdir);
		printf("-7- Search simulation data: data in [%s]\n",xmltestdir);
		printf("--------------------------------------------------------\n");
		printf("-8- Execute Scheduling Result: data in [%s]\n",xmltestdir);
		printf("-0- Exit system.\n");
		printf("--------------------------------------------------------\nInput:");
		scanf("%d",&choice);
		printf("--------------------------------------------------\n");
		if(choice==0 ||choice>8) break;
		
		/*1--4分别表示当前所选择的调度决策类型*/
		actionstatus=choice;
		
		if(choice==5){
			/*生成模拟数据如扩*/
			strcpy(xmldir,xmltestdir);

			/*调用模拟数据生成函数*/
			GenerateData(xmldir);
			
			printf("--------------------------------------------------\n");
			if(choice!=0) printf("See result in %s.\n",xmldir);
			printf("--------------------------------------------------\n\n");
		}
		else if(choice==6)
		{
			strcpy(xmldir,xmltestdir);
			init_shared_data();/*初始化内存*/
			loaddata(xmldir);/*加载XML数据*/
		}
		else if(choice==7)
		{
			searchdata();
		}
		else if(choice==8)
		{

		}
		else{/*choice in 1 to 4*/
			printf("You choose the scheduling action type: %s.\n\n",schetype[choice-1]);
			printf("Please first prepare the xml files in a subdir of dir. [%s], \nand choose a sub dir number as follows, or input 0 to return back:\n",xmlaffdir[choice-1]);
			printf("*--------------------------------------------------*\n");
			printf("* subdir no.  subdir name                          *\n");
			printf("*--------------------------------------------------*\n");
			
			printf("*input the number of test: 18 for /test18/\n");
			
			printf("*--------------------------------------------------*\n");
			printf("*     -0-: return.\n");
			printf("*--------------------------------------------------*\nInput:");
			/*输入选择的目录号*/
			scanf("%d",&selurl);
			
			if(selurl==0) goto start;

			/*生成测试数据所在目录*/
			strcpy(xmldir,xmlrootdir);
			strcat(xmldir,xmlaffdir[choice-1]);
			strcat(xmldir,xmlaffsubdir[selurl-1]);

			/*执行调度决策*/
			do_schedule(xmldir,xmldir);
		
			printf("\n--------------------------------------------------------\n");
			printf("Readxml time:%f. Do schedule time:%f.\n",readxml_time,dosched_time);
			
			if(choice!=0) printf("\nSee result xml3.xml in %s.\n",xmldir);
			printf("--------------------------------------------------------\n\n");
	
		}

	}

	system("pause");
}
