
/* �ļ�ע�� */
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
	
	/*һ����Ҫ����*/
	sharedataPtr sd;/*ָ�������ݽṹ��ָ��*/

	/* Predict if true */
	 int predict;
	
	/*///////////////////////////////////////////////////////////////////*/
	/*������Ҫ����*/
	
	/***1.XML�����йص���*************************************/
	
	/* Read xml from src (from either file or memory) */
    int (*readXML)(int type, char *src, char *dst, char *dir);

    /* Parse xml1 to requestList */
    void (*parseXML1)(char *filename);

    /* Parse xml2 to VMList and PMList */
    void (*parseXML2)(char *filename); 
	
	/* Parse xml21 to p2pList ʱ��-����-����*/
    void (*parseXML21)(char *filename); 
   
	/* Parse xml22 to v2vList VM����-����*/
    void (*parseXML22)(char *filename); 
	
	/* Parse xml3 to actionList ���Ⱦ���ʵʩ�����б�*/
    void (*parseXML3)(char *filename); 	

    /*////////////////////////////////////////////////////////////////////////////*/
	
	/* Construct xml2 based on vmid2vm and pmid2pm  = "data_output.xml"*/
    void (*constructXML2)(char *filename);

	/* Construct xml21 based on p2pList ʱ��-����-����*/
    void (*constructXML21)(char *filename); 

	/* Construct xml22 based on v2vList VM����-����*/
    void (*constructXML22)(char *filename); 

    /* Construct xml3 based on the actionList */
    void (*constructXML3)(char *filename);

    
	/*****************************************************************************/

	/**||||||2.����ģ�������йصĺ���||||||||||||||||||||||*/

	/* Generate PM list data */
	void (*generate_pmlist)(int pm_num, pmPtr lstpm);
	/* Generate VM list data */
	void (*generate_vmlist)(int vm_num, vmPtr lstvm);	
	/* Generate v2v list data -�������������*/
	void (*generate_v2vlist)(int vm_num, float vr);
	/* Generate p2p list data -�����ʱ�Ӿ�������*/
	void (*generate_p2plist)(int pm_num);	
	
	/* Generate all data */
	void (*generate_all_data)(void);

	void (*generate_vmpmlist)(int vmnum);

	/*****************************************************************************/

	/**||||||3.����ϵͳ�йصĺ���||||||||||||||||||||||*/
	
	/* Set default request */
    void (*setDefaultRequest)();

	/*||ִ�е����㷨*/
	void (*execute_sched)(int);
	

	/*****************************************************************************/
	/*��ȡ���жϣ�ϵͳ״̬��*/
	int (*get_sysstat)();		  
	
	
	/*****************************************************************************/

	/**||||||������غ���||||||||||||||||||||||*/

	/*��������Ϊ*/
	void (*do_action)(int action);
	void (*read_xml_vm_pm_data)(); //��ȡXML�������ݣ�����vm and pm��
	void (*read_xml_vm2vm)();      //��ȡXML�����������ϵ���ݣ�
	void (*read_xml_pm2pm_delay)(); //��ȡXML��������˹�ϵ���ݣ�	
	 

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

	/*��ʼ��v2vlist��ָ��vm��ָ��*/
	void (*init_v2vlist)();
	int (*get_vmid_in_vmids)(char * id, int * j);

} cls_st_drscontrol;


/*------------------------------------------------------------------------*/
typedef cls_st_drscontrol* cls_drscontrolPtr;

/*��ʼ�������������*/
void init_drscontrol(cls_drscontrolPtr THIS);

/*thisָ��*/
cls_drscontrolPtr _dcthis;


/*///////////////////////////////////////////////////////////////////*/
/*������Ҫ��������*/

/***1.XML�����йص���*************************************/

/* Read xml from src (from either file or memory) */
int readXML(int type, char *src, char *dst, char *dir);

/* Parse xml1 to requestList */
void parseXML1(char *filename);

/* Parse xml2 to VMList and PMList */
void parseXML2(char *filename); 

/* Parse xml21 to p2pList ʱ��-����-����*/
void parseXML21(char *filename); 

/* Parse xml22 to v2vList VM����-����*/
void parseXML22(char *filename); 

/* Parse xml3 to actionList ���Ⱦ���ʵʩ�����б�*/
void parseXML3(char *filename); 	

/*////////////////////////////////////////////////////////////////////////////*/

/* Construct xml2 based on vmid2vm and pmid2pm  = "data_output.xml"*/
void constructXML2(char *filename );

/* Construct xml21 based on p2pList ʱ��-����-����*/
void constructXML21(char *filename); 

/* Construct xml22 based on v2vList VM����-����*/
void constructXML22(char *filename); 

/* Construct xml3 based on the actionList */
void constructXML3(char *filename);


/*****************************************************************************/

/**||||||2.����ģ�������йصĺ���||||||||||||||||||||||*/

/* Generate PM list data */
void generate_pmlist(int pm_num, pmPtr lstpm);
/* Generate VM list data */
void generate_vmlist(int vm_num, vmPtr lstvm);	
/* Generate v2v list data -�������������*/
void generate_v2vlist(int vm_num, float vr);
/* Generate p2p list data -�����ʱ�Ӿ�������*/
void generate_p2plist(int pm_num);	

/* Generate all data */

void generate_vmpmlist(int vmnum);


/*****************************************************************************/

/**||||||3.����ϵͳ�йصĺ���||||||||||||||||||||||*/

/* Set default request */
void setDefaultRequest();

/*||ִ�е����㷨*/
void execute_sched(int);		



/*****************************************************************************/
/*��ȡ���жϣ�ϵͳ״̬��*/
int get_sysstat();		  


/*****************************************************************************/

/**||||||������غ���||||||||||||||||||||||*/

/*��������Ϊ*/
void do_action(int action);
void read_xml_vm_pm_data(); //��ȡXML�������ݣ�����vm and pm��
void read_xml_vm2vm();      //��ȡXML�����������ϵ���ݣ�
void read_xml_pm2pm_delay(); //��ȡXML��������˹�ϵ���ݣ�	

 

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

/*��ʼ��v2vlist��ָ��vm��ָ��*/
void init_v2vlist();

int get_vmid_in_vmids(char * id, int * j);

#endif
