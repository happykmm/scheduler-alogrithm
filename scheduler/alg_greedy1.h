/*

 
*/


#ifndef _DRS_greedy1_H
#define _DRS_greedy1_H

#include "../lib/drsdata.h"
#include "../lib/drsdef.h"

//struct cls_st_greedy1;

/*网元的最大规模是单逻辑网元512VM，涉及300物理板。
目前场景有一个网元含13物理框*28物理板，
共计364逻辑板。场景二：20框*13板=260逻辑板。*/


typedef struct{

	sharedataPtr sd; /*指向全局数据共享区的指针*/
	
	void (*greedy1)(); /*调度函数*/
	
} cls_st_greedy1;

typedef cls_st_greedy1* cls_greedy1Ptr;

static cls_greedy1Ptr _g1this;

void init_greedy1(cls_st_greedy1 *THIS); /*初始化*/

/*负载均衡调度入口函数*/
void greedy1();

/*/////////////////////////////////////////////////////////////*/

#endif

