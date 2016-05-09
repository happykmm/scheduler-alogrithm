/*
 * @file cls_host.h includes host definitions functions to call publicly
 * @created by jianhaichen <chenjh919@zju.edu.cn>, zhejiang university and huwei l.t.d
 * @modified by jianhaichen at 2013.4.12
 */

#ifndef TDRS_CLS_HOST_H_INCLUDED
#define TDRS_CLS_HOST_H_INCLUDED

#include "drsdata.h"
#include "util.h"

typedef struct {
	st_host host;	
	sharedataPtr sd;

	double (*getUsed)(int name);
	double (*getTotal)(int name);
	double (*getAvail)(int name);
	double (*getReserved)(int name);
	
	double (*get)(int name, int type);
	void (*set)(int name, int type, double val);
	char* (*getSId)();
	char* (*getIp)();
	char* (*getName)(int name);

	pmPtr (*getpmbysid)(char *sid);//获取sid对应的PM
	vmPtr (*getvmbysid)(char *sid);//获取sid对应的VM

} cls_st_host;

typedef cls_st_host* clshostPtr; 

clshostPtr iHost;

void initHost(clshostPtr THIS);

extern char nameOfMetric[DIM][20];

double getUsed(int name);
double getTotal(int name);
double getAvail(int name);
double getReserved(int name);

double getSumCost(); /*先归一处理，各维度值求和作为总的资源量*/
double getAvgCost();/*先归一处理，各维度值求平均作为总的资源量*/
double getWeightedCost();/*先归一处理，各维度值加权求和作为总的资源量*/


double get(int name, int type);
void set(int name, int type, double val);
char *getSId();
char *getIp();
char *getName(int name);

pmPtr getpmbysid(char *sid);//获取sid对应的PM
vmPtr getvmbysid(char *sid);//获取sid对应的VM

#endif