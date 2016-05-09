/*
 * @file cls_pm.h includes host definitions functions to call publicly
 * @ ����PM���ݴ������ģ�飬PM��PM2PM���ݵĴ���
 * @created by jianhaichen <chenjh919@zju.edu.cn>, zhejiang university and huwei l.t.d
 * @modified by jianhaichen at 2013.4.12
 */

#ifndef TDRS_CLS_PMG_H_INCLUDED
#define TDRS_CLS_PMG_H_INCLUDED

#include "drsdata.h"
#include "util.h"
#include "drslib.h"

typedef struct{
	st_pmgroup pg;
	st_share_data *sd;
	int (*getCountPMs)();
	void (*sid2ids)(int group_id);
	void (*get_group_pmids)();
	pmgPtr (*getpmgbysid)(char *sid);

} cls_st_pmgroup;

typedef cls_st_pmgroup* clsstpmgroupPtr;

clsstpmgroupPtr _pmgthis;

void initPmgroup(clsstpmgroupPtr THIS);
int getCountPMs();
void pmg_sid2ids(int group_id);
void get_group_pmids();
pmgPtr getpmgbysid(char *sid);

#endif
