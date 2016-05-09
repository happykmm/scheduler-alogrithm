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

	pmPtr (*getpmbysid)(char *sid);//��ȡsid��Ӧ��PM
	vmPtr (*getvmbysid)(char *sid);//��ȡsid��Ӧ��VM

} cls_st_host;

typedef cls_st_host* clshostPtr; 

clshostPtr iHost;

void initHost(clshostPtr THIS);

extern char nameOfMetric[DIM][20];

double getUsed(int name);
double getTotal(int name);
double getAvail(int name);
double getReserved(int name);

double getSumCost(); /*�ȹ�һ������ά��ֵ�����Ϊ�ܵ���Դ��*/
double getAvgCost();/*�ȹ�һ������ά��ֵ��ƽ����Ϊ�ܵ���Դ��*/
double getWeightedCost();/*�ȹ�һ������ά��ֵ��Ȩ�����Ϊ�ܵ���Դ��*/


double get(int name, int type);
void set(int name, int type, double val);
char *getSId();
char *getIp();
char *getName(int name);

pmPtr getpmbysid(char *sid);//��ȡsid��Ӧ��PM
vmPtr getvmbysid(char *sid);//��ȡsid��Ӧ��VM

#endif