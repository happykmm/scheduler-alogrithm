
/* 文件注释 */
/*
 * @file ganglia.c parse ganglia xml
 * @created by tuantuan <dangoakachan@foxmail.com>
 * @modified by tuantuan at 2011.7.19
 */

#pragma   once  
#ifndef __DATAGENERATOR_H__
#define __DATAGENERATOR_H__

#include <stdlib.h>
#include <time.h>
#include "../lib/drsdata.h"
#include "../lib/drsdef.h"
#include "../lib/util.h"

#include "../lib/drslib.h"

/**********************************************************************/
/**修改：陈建海 20130722********************************************************************/
typedef struct{
	int id;
	
	sharedataPtr sd; /*共享数据结构指针*/

	void	(*set_seed)(void);				//获取随机数种子
	double	(*random_021)(void);				/*随机生成区间0-1内的实数*/
	double	(*random_a2b)(double ,double );	/*随机生成区间a-b内的实数*/
	int		(*random_int_ab)(int ,int );	/*随机生成整数a或b*/
	int		(*ifiprepeat)(int ,int ,int );		/*检测随机生成的ip地址是否重复*/
	int		(*ifv2vrepeat)(int, int, int);		/*检测随机生成的vm2vm是否重复*/
	int		(*updatepm)(int ,int );					/*产生一个虚拟机后,更新物理机*/
	int		(*create_pm)(int,int,int);					/*随机生成物理机*/
	int		(*create_group)(int from,int num);  /*生成group */
	int		(*create_frame)(int num); /*生成frame*/
	void	(*create_vm)(double ,double,int,int,int,int);/*随机生成虚拟机*/
	void	(*create_v2v)(double lower, double upper, int vmnum, double ratio);/*随机生成vm流量关系*/
	void	(*create_v2v_no_aff)(int affnum, int afftype);
	void	(*create_p2p)(double lower, double upper,int pmnum);		/*随机生成时延矩阵*/
	void	(*clear_res)(void);						/*将reserved全置为0*/
	void	(*count)(void);						/*统计热点机数和冷点机数*/
	void	(*set_stat)(void);						/*将非空物理机的stat置为1,空物理机置为0*/
	//void	(*generator_fun)(void);				/*模拟数据生成模块入口*/
	void (*generate_data_by_vms)(int vmnum, float a,float b, float hv);
	void (*create_v2v_by_aff_type)(float a, float b, int affnum, int afftype);
	void (*create_v2v_by_ta)(float a,float b,int affnum,int aftype);
	void (*addtov2vlist)(vmPtr p_vm1,vmPtr p_vm2,float a,float b,int aftype);
	void (*create_empty_pm)(int pmnum);
	void (*createPMV)();
	void (*set_empty_pm)(pmPtr pmlist,int num);
	void (*create_p2p_tree_topo)(double lower, double upper,int pmnum);

} cls_st_datagenerator;

typedef cls_st_datagenerator* cls_generatorPtr;

/*内部this指针*/
cls_generatorPtr _gthis;

/**************************************************/
/* initialize class parameters*/
void init_generator(cls_generatorPtr THIS);

/**************************************************/
/* declarations of functions for this class*/
void set_seed(void);						/*获取随机数种子*/
int create_frame(int num);
int create_group(int num);


double random_021(void);							/*//随机生成区间0-1内的实数*/
double random_a2b(double,double );			/*//随机生成区间a-b内的实数*/
int	random_int_ab(int,int);			/*//随机生成整数a或b*/
int	ifiprepeat(int,int ,int);		/*//检测随机生成的ip地址是否重复*/
int	ifv2vrepeat(int, int, int);		/*		//检测随机生成的vm2vm是否重复*/
int	updatepm(int ,int);				/*	//产生一个虚拟机后,更新物理机*/
int create_pm(int,int,int);				/*//随机生成物理机*/
void create_vm(double ,double,int,int,int,int);		/*	//随机生成虚拟机*/
void create_v2v(double lower, double upper, int vmnum, double ratio);/*//随机生成vm流量关系*/
void create_v2v_no_aff(int affnum, int afftype);
//void	generator_fun(void);			/*	//模拟数据生成模块入口*/
void create_p2p(double lower, double upper,int pmnum);  /*随机生成时延矩阵*/
void clear_res(void);						/*统计热点机数和冷点机数*/
void count(void);						/*统计热点机数和冷点机数*/
void set_stat(void);					/*将非空物理机的stat置为1,空物理机置为0*/

void create_empty_pm(int from,int pmnum);
void set_empty_pm(pmPtr pmlist,int num);

void createPMV();
//int canPack(vmPtr p_vm, pmPtr p_pm, double hcv);

void generate_data_by_vms(int vmnum, float a,float b,float hv);
void create_v2v_by_aff_type(float a, float b, int affnum, int afftype);
void create_v2v_by_ta(float a,float b,int affnum,int aftype);
void addtov2vlist(vmPtr p_vm1,vmPtr p_vm2,float a,float b,int aftype);
void create_p2p_tree_topo(double lower, double upper,int pmnum);
/**************************************************/
int conflict(int vmid1,int vmid2,int afftyp);


#endif 
