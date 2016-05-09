/*

 
*/


#ifndef _DRS_greedy2_H
#define _DRS_greedy2_H

#include "../lib/drsdata.h"
#include "../lib/drsdef.h"

//struct cls_st_greedy2;

/*网元的最大规模是单逻辑网元512VM，涉及300物理板。
目前场景有一个网元含13物理框*28物理板，
共计364逻辑板。场景二：20框*13板=260逻辑板。*/


typedef struct{

	sharedataPtr sd; /*指向全局数据共享区的指针*/
	
	void (*greedy2)(); /*调度函数*/
	
} cls_st_greedy2;

typedef cls_st_greedy2* cls_greedy2Ptr;

static cls_greedy2Ptr _g2this;

void init_greedy2(cls_st_greedy2 *THIS); /*初始化*/

/*负载均衡调度入口函数*/
void greedy2();

/*/////////////////////////////////////////////////////////////*/

#endif

