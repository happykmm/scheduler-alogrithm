

/*
 * @file cls_pm.h includes host definitions functions to call publicly
 * @ 包含PM数据处理的类模块，PM，PM2PM数据的处理
 * @created by jianhaichen <chenjh919@zju.edu.cn>, zhejiang university and huwei l.t.d
 * @modified by jianhaichen at 2013.4.12
 */

#ifndef TDRS_CLS_NET_H_INCLUDED
#define TDRS_CLS_NET_H_INCLUDED

#include "drsdata.h"
#include "util.h"
#include "drslib.h"

#define MAX_DELAY 999999

typedef struct{
	//st_pm pm;
	
	st_share_data *sd;
	void (*getdelay)();
	float (*get_pm_delay)(int pm1,int pm2);
	void (*get_pmport_at_switch)();
	void (*update_all_p2s)();
	void (*update_all_sw)();
	void (*update_all_s2s)();
	void (*update_net)();
	void (*preparePMnearest)();

} cls_st_net;

typedef cls_st_net* clsstnetPtr;

typedef struct{
	int at_sw;
	float delay;
} st_p2s_delay;

clsstnetPtr nthis;

void initNet(clsstnetPtr THIS);

void getdelay();/*获取switch时延函数*/
float get_pm_delay(int pm1,int pm2);
void get_pmport_at_switch();
void update_all_p2s();
void update_all_sw();
void update_all_s2s();
void update_net();
void preparePMnearest();

#endif


