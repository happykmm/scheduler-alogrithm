/*

 
*/


#ifndef _DRS_consolidate_H
#define _DRS_consolidate_H

#include "../lib/drsdata.h"
#include "../lib/drsdef.h"

//struct cls_st_consolidate;

/*��Ԫ������ģ�ǵ��߼���Ԫ512VM���漰300����塣
Ŀǰ������һ����Ԫ��13�����*28����壬
����364�߼��塣��������20��*13��=260�߼��塣*/


typedef struct{

	sharedataPtr sd; /*ָ��ȫ�����ݹ�������ָ��*/
	
	void (*consolidate)(); /*���Ⱥ���*/
	
} cls_st_consolidate;

typedef cls_st_consolidate* cls_consolidatePtr;

static cls_consolidatePtr _c1this;

void init_consolidate(cls_st_consolidate *THIS); /*��ʼ��*/

/*���ؾ��������ں���*/
void consolidate();

/*/////////////////////////////////////////////////////////////*/

#endif

