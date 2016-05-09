#pragma   once  
#ifndef _DRS_DRSCTRL_H
#define _DRS_DRSCTRL_H

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

int actionstatus; /*调度状态值：1-初始放置，2-负载均衡，3-热点消除，4-冷点整合*/
double readxml_time,dosched_time,outxml_time;/*总时间*/


/*执行调度决策*/
void do_schedule(char *xmlinputdir,char *outputdir);

void do_schedule_by_dir(char *rootdir);



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

/*查询数据*/
int searchdata();

/*//////////////////////////////////////////*/
/*生成模拟数据函数*/
void GenerateData(char *RootDir);
/*生成PM拓扑数据*/
void Generate_TOPO();
/*生成亲和性数据*/
void Generate_AFF();
/*生成框数据*/
void Generate_FRAME();
/*生成PM组数据*/
void Generate_PMG();
/*生成PMVM数据*/
void Generate_PMVM();
/*置PM空载*/
void Generate_EPTPM();
/*生成XML*/
void Generate_XML(char *rootdir);

/*手工输入生成*/
void Generate_Manul_AFF();

/*随机自动产生*/
void Generate_AutoRND_AFF();

/*//////////////////////////////////////////*/
void showpmglist();/*显示所有组列表*/
void showframelist();/*显示所有框列表*/
/*显示所有的v2v亲和性信息*/
void showaffinitylist();

/*执行调度决策*/
void do_schedule(char *xmlinputdir,char *outputdir);


int execute(char *p_xml1, char *p_xml2, char *p_xml21, char *p_xml22, char *p_xml3);

/*按照调度选择进行整体调度测试*/
void make_whole_scheduling();

/*启动模拟的drs*/
void start_drs_sys();

#endif