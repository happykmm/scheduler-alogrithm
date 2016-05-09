/*
 * @file cls_pm.h includes host definitions functions to call publicly
 * @ 包含PM数据处理的类模块，PM，PM2PM数据的处理
 * @created by jianhaichen <chenjh919@zju.edu.cn>, zhejiang university and huwei l.t.d
 * @modified by jianhaichen at 2013.4.12
 */

#ifndef TDRS_CLS_PM_H_INCLUDED
#define TDRS_CLS_PM_H_INCLUDED

#include "drsdata.h"
#include "util.h"
#include "drslib.h"
//#include "../dataprocess/generator.h"

typedef struct{
	st_pm pm;
	cls_st_host shost;
	sharedataPtr sd;

	int (*getCountOfVMs)();
	int (*getVMS)();
	void (*set_fr_gr_id)();
} cls_st_pm;

typedef cls_st_pm* clsstpmPtr;

clsstpmPtr pthis;




void initPm(clsstpmPtr THIS);
int getCountOfVMs();
int getVMS();
void set_fr_gr_id();

#endif
