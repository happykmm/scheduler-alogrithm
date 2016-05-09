
/* �ļ�ע�� */
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
/**�޸ģ��½��� 20130722********************************************************************/
typedef struct{
	int id;
	
	sharedataPtr sd; /*�������ݽṹָ��*/

	void	(*set_seed)(void);				//��ȡ���������
	double	(*random_021)(void);				/*�����������0-1�ڵ�ʵ��*/
	double	(*random_a2b)(double ,double );	/*�����������a-b�ڵ�ʵ��*/
	int		(*random_int_ab)(int ,int );	/*�����������a��b*/
	int		(*ifiprepeat)(int ,int ,int );		/*���������ɵ�ip��ַ�Ƿ��ظ�*/
	int		(*ifv2vrepeat)(int, int, int);		/*���������ɵ�vm2vm�Ƿ��ظ�*/
	int		(*updatepm)(int ,int );					/*����һ���������,���������*/
	int		(*create_pm)(int,int,int);					/*������������*/
	int		(*create_group)(int from,int num);  /*����group */
	int		(*create_frame)(int num); /*����frame*/
	void	(*create_vm)(double ,double,int,int,int,int);/*������������*/
	void	(*create_v2v)(double lower, double upper, int vmnum, double ratio);/*�������vm������ϵ*/
	void	(*create_v2v_no_aff)(int affnum, int afftype);
	void	(*create_p2p)(double lower, double upper,int pmnum);		/*�������ʱ�Ӿ���*/
	void	(*clear_res)(void);						/*��reservedȫ��Ϊ0*/
	void	(*count)(void);						/*ͳ���ȵ������������*/
	void	(*set_stat)(void);						/*���ǿ��������stat��Ϊ1,���������Ϊ0*/
	//void	(*generator_fun)(void);				/*ģ����������ģ�����*/
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

/*�ڲ�thisָ��*/
cls_generatorPtr _gthis;

/**************************************************/
/* initialize class parameters*/
void init_generator(cls_generatorPtr THIS);

/**************************************************/
/* declarations of functions for this class*/
void set_seed(void);						/*��ȡ���������*/
int create_frame(int num);
int create_group(int num);


double random_021(void);							/*//�����������0-1�ڵ�ʵ��*/
double random_a2b(double,double );			/*//�����������a-b�ڵ�ʵ��*/
int	random_int_ab(int,int);			/*//�����������a��b*/
int	ifiprepeat(int,int ,int);		/*//���������ɵ�ip��ַ�Ƿ��ظ�*/
int	ifv2vrepeat(int, int, int);		/*		//���������ɵ�vm2vm�Ƿ��ظ�*/
int	updatepm(int ,int);				/*	//����һ���������,���������*/
int create_pm(int,int,int);				/*//������������*/
void create_vm(double ,double,int,int,int,int);		/*	//������������*/
void create_v2v(double lower, double upper, int vmnum, double ratio);/*//�������vm������ϵ*/
void create_v2v_no_aff(int affnum, int afftype);
//void	generator_fun(void);			/*	//ģ����������ģ�����*/
void create_p2p(double lower, double upper,int pmnum);  /*�������ʱ�Ӿ���*/
void clear_res(void);						/*ͳ���ȵ������������*/
void count(void);						/*ͳ���ȵ������������*/
void set_stat(void);					/*���ǿ��������stat��Ϊ1,���������Ϊ0*/

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
