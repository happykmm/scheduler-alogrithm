#pragma   once  
#ifndef _DRS_DRSCTRL_H
#define _DRS_DRSCTRL_H

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

int actionstatus; /*����״ֵ̬��1-��ʼ���ã�2-���ؾ��⣬3-�ȵ�������4-�������*/
double readxml_time,dosched_time,outxml_time;/*��ʱ��*/


/*ִ�е��Ⱦ���*/
void do_schedule(char *xmlinputdir,char *outputdir);

void do_schedule_by_dir(char *rootdir);



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

/*��ѯ����*/
int searchdata();

/*//////////////////////////////////////////*/
/*����ģ�����ݺ���*/
void GenerateData(char *RootDir);
/*����PM��������*/
void Generate_TOPO();
/*�����׺�������*/
void Generate_AFF();
/*���ɿ�����*/
void Generate_FRAME();
/*����PM������*/
void Generate_PMG();
/*����PMVM����*/
void Generate_PMVM();
/*��PM����*/
void Generate_EPTPM();
/*����XML*/
void Generate_XML(char *rootdir);

/*�ֹ���������*/
void Generate_Manul_AFF();

/*����Զ�����*/
void Generate_AutoRND_AFF();

/*//////////////////////////////////////////*/
void showpmglist();/*��ʾ�������б�*/
void showframelist();/*��ʾ���п��б�*/
/*��ʾ���е�v2v�׺�����Ϣ*/
void showaffinitylist();

/*ִ�е��Ⱦ���*/
void do_schedule(char *xmlinputdir,char *outputdir);


int execute(char *p_xml1, char *p_xml2, char *p_xml21, char *p_xml22, char *p_xml3);

/*���յ���ѡ�����������Ȳ���*/
void make_whole_scheduling();

/*����ģ���drs*/
void start_drs_sys();

#endif