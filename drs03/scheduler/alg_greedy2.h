/*

 
*/


#ifndef _DRS_greedy2_H
#define _DRS_greedy2_H

#include "../lib/drsdata.h"
#include "../lib/drsdef.h"

//struct cls_st_greedy2;

/*��Ԫ������ģ�ǵ��߼���Ԫ512VM���漰300����塣
Ŀǰ������һ����Ԫ��13�����*28����壬
����364�߼��塣��������20��*13��=260�߼��塣*/


typedef struct{

	sharedataPtr sd; /*ָ��ȫ�����ݹ�������ָ��*/
	
	void (*greedy2)(); /*���Ⱥ���*/
	
} cls_st_greedy2;

typedef cls_st_greedy2* cls_greedy2Ptr;

static cls_greedy2Ptr _g2this;

void init_greedy2(cls_st_greedy2 *THIS); /*��ʼ��*/

/*���ؾ��������ں���*/
void greedy2();

/*/////////////////////////////////////////////////////////////*/

#endif

