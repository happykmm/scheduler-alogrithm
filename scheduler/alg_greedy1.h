/*

 
*/


#ifndef _DRS_greedy1_H
#define _DRS_greedy1_H

#include "../lib/drsdata.h"
#include "../lib/drsdef.h"

//struct cls_st_greedy1;

/*��Ԫ������ģ�ǵ��߼���Ԫ512VM���漰300����塣
Ŀǰ������һ����Ԫ��13�����*28����壬
����364�߼��塣��������20��*13��=260�߼��塣*/


typedef struct{

	sharedataPtr sd; /*ָ��ȫ�����ݹ�������ָ��*/
	
	void (*greedy1)(); /*���Ⱥ���*/
	
} cls_st_greedy1;

typedef cls_st_greedy1* cls_greedy1Ptr;

static cls_greedy1Ptr _g1this;

void init_greedy1(cls_st_greedy1 *THIS); /*��ʼ��*/

/*���ؾ��������ں���*/
void greedy1();

/*/////////////////////////////////////////////////////////////*/

#endif

