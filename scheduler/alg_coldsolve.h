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

/*网元的最大规模是单逻辑网元512VM，涉及300物理板。
目前场景有一个网元含13物理框*28物理板，
共计364逻辑板。场景二：20框*13板=260逻辑板。*/

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
	sharedataPtr sd; /*指向全局数据共享区的指针*/

	/*算法参数*/
	float hot,cold,warm; /*来自XML11*/

	void (*coldsolve)(float,float,float); /*调度函数*/
} cls_st_coldsolve;

typedef cls_st_coldsolve* cls_coldsolvePtr;
	cls_st_sys st;
	cls_st_vm_group vmg_cut;
static cls_coldsolvePtr _csthis;

void init_coldsolve(cls_coldsolvePtr THIS); /*初始化*/

/*初始化算法配置参数来自XML11,CJH ADD*/
void setRequest_coldsolve(cls_coldsolvePtr THIS);

/*冷点整合调度入口函数*/
void coldsolve(float hot,float cold,float warm);

int if_can_off (int pm_id, st_pm pmlist[ ]);

#endif 