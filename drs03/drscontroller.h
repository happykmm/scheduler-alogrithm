
/* 文件注释 */
/*
 * @file drscontroll.h realize drs controll functions
 * @created by  jianhaichen at 2013.05.11
 * @modified by Jianhai Chen at 2013.05.13
 */

#ifndef _DRS_CONTROLLOR_H
#define _DRS_CONTROLLOR_H 
#include <libxml/tree.h>
#include "lib/drsdata.h"
#include "lib/drsdef.h"
#include "stdio.h"
#include "./dataprocess/generator.h"
#include "./dataprocess/xmlprocess.h"
#include "scheduler/scheduler.h"


//int _execute(const char *xml1Src, const char *xml2Src, int calc_util);

typedef struct
{
	/*///////////////////////////////////////*////////////////////////////////////////////*///////*/
	
	/*一、主要属性*/
	sharedataPtr sd;/*指向共享数据结构的指针*/

	/* Predict if true */
	 int predict;
	
	/*///////////////////////////////////////////////////////////////////*/
	/*二、主要函数*/
	
	/***1.XML解析有关的类*************************************/
	
	/* Read xml from src (from either file or memory) */
    int (*readXML)(int type, char *src, char *dst, char *dir);

    /* Parse xml1 to requestList */
    void (*parseXML1)(char *filename);

    /* Parse xml2 to VMList and PMList */
    void (*parseXML2)(char *filename); 
	
	/* Parse xml21 to p2pList 时延-距离-矩阵*/
    void (*parseXML21)(char *filename); 
   
	/* Parse xml22 to v2vList VM流量-矩阵*/
    void (*parseXML22)(char *filename); 
	
	/* Parse xml3 to actionList 调度决策实施操作列表*/
    void (*parseXML3)(char *filename); 	

    /*////////////////////////////////////////////////////////////////////////////*/
	
	/* Construct xml2 based on vmid2vm and pmid2pm  = "data_output.xml"*/
    void (*constructXML2)(char *filename);

	/* Construct xml21 based on p2pList 时延-距离-矩阵*/
    void (*constructXML21)(char *filename); 

	/* Construct xml22 based on v2vList VM流量-矩阵*/
    void (*constructXML22)(char *filename); 

    /* Construct xml3 based on the actionList */
    void (*constructXML3)(char *filename);

    
	/*****************************************************************************/

	/**||||||2.生成模拟数据有关的函数||||||||||||||||||||||*/

	/* Generate PM list data */
	void (*generate_pmlist)(int pm_num, pmPtr lstpm);
	/* Generate VM list data */
	void (*generate_vmlist)(int vm_num, vmPtr lstvm);	
	/* Generate v2v list data -虚拟机流量数据*/
	void (*generate_v2vlist)(int vm_num, float vr);
	/* Generate p2p list data -物理机时延距离数据*/
	void (*generate_p2plist)(int pm_num);	
	
	/* Generate all data */
	void (*generate_all_data)(void);

	void (*generate_vmpmlist)(int vmnum);

	/*****************************************************************************/

	/**||||||3.调度系统有关的函数||||||||||||||||||||||*/
	
	/* Set default request */
    void (*setDefaultRequest)();

	/*||执行调度算法*/
	void (*execute_sched)(int);
	

	/*****************************************************************************/
	/*获取（判断）系统状态；*/
	int (*get_sysstat)();		  
	
	
	/*****************************************************************************/

	/**||||||其他相关函数||||||||||||||||||||||*/

	/*控制器行为*/
	void (*do_action)(int action);
	void (*read_xml_vm_pm_data)(); //读取XML负载数据，包括vm and pm；
	void (*read_xml_vm2vm)();      //读取XML虚拟机流量关系数据；
	void (*read_xml_pm2pm_delay)(); //获取XML物理机拓扑关系数据；	
	 

    /* Append action list to existing list */

    /* Update request and data 
     void (*updateRequest)(const char *user);
     int (*updateData)(const char *user);*/
    /* Process all request successively */
    void (*processRequests)();
    /* Call algorithm */
    /* Debug data contained in controllor */
    void (*debugData)(); 

	void (*show_pmlist)();
	void (*show_vmlist)();

	/*初始化v2vlist内指向vm的指针*/
	void (*init_v2vlist)();
	int (*get_vmid_in_vmids)(char * id, int * j);

} cls_st_drscontrol;


/*------------------------------------------------------------------------*/
typedef cls_st_drscontrol* cls_drscontrolPtr;

/*初始化控制器类变量*/
void init_drscontrol(cls_drscontrolPtr THIS);

/*this指针*/
cls_drscontrolPtr _dcthis;


/*///////////////////////////////////////////////////////////////////*/
/*二、主要函数声明*/

/***1.XML解析有关的类*************************************/

/* Read xml from src (from either file or memory) */
int readXML(int type, char *src, char *dst, char *dir);

/* Parse xml1 to requestList */
void parseXML1(char *filename);

/* Parse xml2 to VMList and PMList */
void parseXML2(char *filename); 

/* Parse xml21 to p2pList 时延-距离-矩阵*/
void parseXML21(char *filename); 

/* Parse xml22 to v2vList VM流量-矩阵*/
void parseXML22(char *filename); 

/* Parse xml3 to actionList 调度决策实施操作列表*/
void parseXML3(char *filename); 	

/*////////////////////////////////////////////////////////////////////////////*/

/* Construct xml2 based on vmid2vm and pmid2pm  = "data_output.xml"*/
void constructXML2(char *filename );

/* Construct xml21 based on p2pList 时延-距离-矩阵*/
void constructXML21(char *filename); 

/* Construct xml22 based on v2vList VM流量-矩阵*/
void constructXML22(char *filename); 

/* Construct xml3 based on the actionList */
void constructXML3(char *filename);


/*****************************************************************************/

/**||||||2.生成模拟数据有关的函数||||||||||||||||||||||*/

/* Generate PM list data */
void generate_pmlist(int pm_num, pmPtr lstpm);
/* Generate VM list data */
void generate_vmlist(int vm_num, vmPtr lstvm);	
/* Generate v2v list data -虚拟机流量数据*/
void generate_v2vlist(int vm_num, float vr);
/* Generate p2p list data -物理机时延距离数据*/
void generate_p2plist(int pm_num);	

/* Generate all data */

void generate_vmpmlist(int vmnum);


/*****************************************************************************/

/**||||||3.调度系统有关的函数||||||||||||||||||||||*/

/* Set default request */
void setDefaultRequest();

/*||执行调度算法*/
void execute_sched(int);		



/*****************************************************************************/
/*获取（判断）系统状态；*/
int get_sysstat();		  


/*****************************************************************************/

/**||||||其他相关函数||||||||||||||||||||||*/

/*控制器行为*/
void do_action(int action);
void read_xml_vm_pm_data(); //读取XML负载数据，包括vm and pm；
void read_xml_vm2vm();      //读取XML虚拟机流量关系数据；
void read_xml_pm2pm_delay(); //获取XML物理机拓扑关系数据；	

 

/* Append action list to existing list */

/* Update request and data 
 void (*updateRequest)(const char *user);
 int (*updateData)(const char *user);*/
/* Process all request successively */
void processRequests();
/* Call algorithm */
/* Debug data contained in controllor */
void debugData(); 

void show_pmlist();

void show_vmlist();

/*初始化v2vlist内指向vm的指针*/
void init_v2vlist();

int get_vmid_in_vmids(char * id, int * j);

#endif
