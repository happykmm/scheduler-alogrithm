/*

 
*/


#ifndef _DRS_consolidate2_H
#define _DRS_consolidate2_H

#include "../lib/drsdata.h"
#include "../lib/drsdef.h"

//struct cls_st_consolidate2;

/*��Ԫ������ģ�ǵ��߼���Ԫ512VM���漰300����塣
Ŀǰ������һ����Ԫ��13�����*28����壬
����364�߼��塣��������20��*13��=260�߼��塣*/


typedef struct{

	sharedataPtr sd; /*ָ��ȫ�����ݹ�������ָ��*/
	
	void (*consolidate2)(); /*���Ⱥ���*/
	
} cls_st_consolidate2;

typedef cls_st_consolidate2* cls_consolidate2Ptr;

static cls_consolidate2Ptr _c2this;

void init_consolidate2(cls_st_consolidate2 *THIS); /*��ʼ��*/

/*���ؾ��������ں���*/
void consolidate2();

/*/////////////////////////////////////////////////////////////*/

#endif

