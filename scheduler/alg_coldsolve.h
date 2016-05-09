#ifndef __ALG_COLDSOLVE_H__
#define __ALG_COLDSOLVE_H__

#include "../lib/drsdata.h"
#include "../lib/drsdef.h"
#include "../lib/util.h"
//#include "../lib/cls_sys.h"
//#include "../lib/cls_vmgroup.h"
#include "../lib/drslib.h"

//#include <stdio.h>
//#include <stdlib.h>

//struct cls_st_consolidate;

/*��Ԫ������ģ�ǵ��߼���Ԫ512VM���漰300����塣
Ŀǰ������һ����Ԫ��13�����*28����壬
����364�߼��塣��������20��*13��=260�߼��塣*/

/*
typedef struct{
	int id;
	double tot;
	double total_used;
} num;
*/
/*
typedef struct{
	int old_at;
	int now_to;
	int id;
} dec;
*/

typedef struct{
	sharedataPtr sd; /*ָ��ȫ�����ݹ�������ָ��*/

	/*�㷨����*/
	float hot,cold,warm; /*����XML11*/

	void (*coldsolve)(float,float,float); /*���Ⱥ���*/
} cls_st_coldsolve;

typedef cls_st_coldsolve* cls_coldsolvePtr;
	cls_st_sys st;
	cls_st_vm_group vmg_cut;
static cls_coldsolvePtr _csthis;

void init_coldsolve(cls_coldsolvePtr THIS); /*��ʼ��*/

/*��ʼ���㷨���ò�������XML11,CJH ADD*/
void setRequest_coldsolve(cls_coldsolvePtr THIS);

/*������ϵ�����ں���*/
void coldsolve(float hot,float cold,float warm);

int if_can_off (int pm_id, st_pm pmlist[ ]);

#endif 