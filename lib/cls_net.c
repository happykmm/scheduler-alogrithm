/*
 * @file cls_net.c includes net class functions to call publicly
 * 
 * @created by jianhaichen <chenjh919@zju.edu.cn>, zhejiang university and huwei l.t.d
 * @modified by jianhaichen at 2013.4.12
 */

#include "cls_net.h"




//int delay_pm[100][100];//端口的
st_p2s_delay port2sw[PM_TOTAL][4];
double distP2P[PM_TOTAL];
int order[PM_TOTAL];
void initNet(cls_st_net* THIS)
{
	nthis=THIS;	
	
	THIS->getdelay=getdelay;
	THIS->get_pm_delay=get_pm_delay;
	THIS->get_pmport_at_switch = get_pmport_at_switch;
	THIS->update_all_p2s = update_all_p2s;
	THIS->update_all_s2s = update_all_s2s;
	THIS->update_all_sw = update_all_sw;
	THIS->update_net = update_net;
	THIS->preparePMnearest = preparePMnearest;
}
int distCmp(int i,int j){
	return distP2P[i]<distP2P[j];
}
void preparePMnearest(){
	int i,j,k;
	pmPtr pm;
	cls_st_sys sys;
	initSys(&sys);
	
	for (i = 0; i< nthis->sd->pm_total; i++){
		pm  = &nthis->sd->pmlist[i];
		for (j=0;j< nthis->sd->pm_total; j++){
			distP2P[j] = get_pm_delay(i, j);
			order[j] = j;
		}
		sys.sort(order,order+nthis->sd->pm_total, distCmp);
		pm->nearPM_total = min(PM_TOTAL, nthis->sd->pm_total);
		for (j=0 ; j< pm->nearPM_total ;j++)
			pm->nearPM[j] = order[j];
	}


}

float get_pm_delay(int pm1,int pm2)
{
	float delay = MAX_DELAY;//先假设时延无穷大
	float delay_p2s1,delay_p2s2;
	int i,j;
	int sw1,sw2;
	int sw_ids1[4],sw_ids2[4];
	int port;
	if(pm1 == pm2) return 0;
	for(i = 0;i<4;i++)
	{
		sw1 = port2sw[pm1][i].at_sw;
		delay_p2s1 = port2sw[pm1][i].delay;
		if(sw1 != -1)
		{
			for(j = 0;j<4;j++)
			{
				sw2 = port2sw[pm2][j].at_sw;
				delay_p2s2 = port2sw[pm2][j].delay;
				if(sw2 != -1 && nthis->sd->swdelay[sw1][sw2] + delay_p2s1 + delay_p2s2 < delay) 
					//检索两台PM间可能存在的最多16种连接方式，寻找最小时延的连接方式
					delay = nthis->sd->swdelay[sw1][sw2] + delay_p2s1 + delay_p2s2;
			}
		}
	}
	return delay;
}



void get_pmport_at_switch()
{
	int i,j;
	int pm_id,port_id,sw_id;
	float delay;

	for(i=0;i<nthis->sd->pm_total;i++)
		for(j=0;j<4;j++)
			port2sw[i][j].at_sw = -1;

	for(i=0;i<nthis->sd->p2s_total;i++)
	{
		pm_id = nthis->sd->p2slist[i].pm_id;
		port_id =  nthis->sd->p2slist[i].port_id;
		sw_id = nthis->sd->p2slist[i].switch_id;
		delay = nthis->sd->p2slist[i].delay;
		port2sw[pm_id][port_id].at_sw = sw_id;
		port2sw[pm_id][port_id].delay = delay;
	}

}

/*V重Dijkstra算法求最短路径*/

float adj_matrix[SW_TOTAL][SW_TOTAL];
void getdelay()
{
	int i,j,k,t;
	int in_set[SW_TOTAL];
	int sw_total;
	int src,dst;
	int temp_sw;
	float delay_now;
	
	float (*swdelay)[SW_TOTAL];

	sw_total = nthis->sd->sw_total;
	swdelay = nthis->sd->swdelay;
	for(i=0;i<sw_total;i++)
		for(j=0;j<sw_total;j++)
		{
			if(i!=j) adj_matrix[i][j] = swdelay[i][j] = MAX_DELAY;
			else adj_matrix[i][j] = swdelay[i][j] = 0;
		}
	for(i=0;i<nthis->sd->s2s_total;i++)
	{
		src = nthis->sd->s2slist[i].sw_src_id;
		dst = nthis->sd->s2slist[i].sw_dst_id;
		adj_matrix[src][dst] = adj_matrix[dst][src] = nthis->sd->s2slist[i].delay;
		swdelay[src][dst] = swdelay[dst][src] = nthis->sd->s2slist[i].delay;
	}
	for(i=0;i<sw_total;i++)
	{
		for(j=0;j<sw_total;j++) in_set[j] = 0;
		in_set[i] = 1;
		for(j=1;j<sw_total;j++)
		{
			t = -1;
			delay_now = MAX_DELAY;
			for(k=0;k<sw_total;k++)
			{
				if(in_set[k]==0 && swdelay[i][k] < delay_now)
				{
					t = k;
					delay_now = swdelay[i][k];
				}
			}
			if (t==-1) break;
			in_set[t] = 1;
			for(k=0;k<sw_total;k++)
			{
				if(in_set[k]==0 && swdelay[i][t] + adj_matrix[t][k] < swdelay[i][k])
				{
					swdelay[i][k] = swdelay[i][t] + adj_matrix[t][k];
				}
			}
		}
	}
}

/*void getdelay()
{
	int i,j;
	int queue[100];
	int head,tail;
	int delay_now;
	int sw_total;
	int src,dst;
	int vis[SW_TOTAL];//广度遍历访问标记
	sw_total = nthis->sd->sw_total;
	for(i=0;i<sw_total;i++)
		for(j=0;j<sw_total;j++)
		{
			nthis->sd->swdelay[i][j] = -1;
		}
	for(i=0;i<nthis->sd->s2s_total;i++)
	{
		src = nthis->sd->s2slist[i].sw_src_id;
		dst = nthis->sd->s2slist[i].sw_dst_id;
		nthis->sd->swdelay[src][dst] = nthis->sd->swdelay[dst][src] = 0;
	}
	for(i=0;i<sw_total;i++)//对所有switch进行广度遍历
	{
		for(j=0;j<sw_total;j++)
		{
			vis[j] = 0;//初始化访问标记
		}
		vis[i] = 1;
		head = tail = 0;//当前端口进入队列
		queue[0] = i;

		while(head <= tail)
		{
			delay_now = nthis->sd->swdelay[i][queue[head]] + 1;//当前时延为队首节点时延加一
			for(j=0;j<sw_total;j++)//将未访问的父节点加入队列
			{
				src = queue[head];
				dst = j;
				if(nthis->sd->swdelay[src][dst] == 0 && !vis[dst])
				{
					queue[++tail] = dst;
					vis[dst] = 1;
					nthis->sd->swdelay[i][dst] = nthis->sd->swdelay[dst][i] = delay_now;
				}
			}
			head++;//队首后移
		}
	}
}*/

void update_all_p2s()
{
	int i,j;
	int pm_id,sw_id;
	cls_st_host cls_host;
	initHost(&cls_host);
	cls_host.sd = nthis->sd;
	for(i=0;i<nthis->sd->p2s_total;i++)
	{
		pm_id = cls_host.getpmbysid(nthis->sd->p2slist[i].pm_sid)->host.id;
		for(j=0;j<nthis->sd->sw_total;j++)
		{
			if(strcmp(nthis->sd->swlist[j].sid,nthis->sd->p2slist[i].switch_sid) == 0)
			{
				sw_id = nthis->sd->swlist[j].id;
				break;
			}
		}
		nthis->sd->p2slist[i].pm_id = pm_id;
		nthis->sd->p2slist[i].switch_id = sw_id;
		nthis->sd->p2slist[i].p_pm = &nthis->sd->pmlist[pm_id];
		nthis->sd->p2slist[i].p_sw = &nthis->sd->swlist[sw_id];
	}
}

void update_all_sw()
{
	int i;
	for(i=0;i<nthis->sd->sw_total;i++)
	{
		nthis->sd->swlist[i].id = i;
	}
}

void update_all_s2s()
{
	int i,j;
	int src_id,dst_id;
	for(i=0;i<nthis->sd->s2s_total;i++)
	{
		for(j=0;j<nthis->sd->sw_total;j++)
		{
			if(strcmp(nthis->sd->swlist[j].sid,nthis->sd->s2slist[i].sw_src_sid) == 0)
			{
				src_id = nthis->sd->swlist[j].id;
				break;
			}
		}
		for(j=0;j<nthis->sd->sw_total;j++)
		{
			if(strcmp(nthis->sd->swlist[j].sid,nthis->sd->s2slist[i].sw_dst_sid) == 0)
			{
				dst_id = nthis->sd->swlist[j].id;
				break;
			}
		}
		nthis->sd->s2slist[i].sw_src_id = src_id;
		nthis->sd->s2slist[i].sw_dst_id = dst_id;
		nthis->sd->s2slist[i].p_src_sw = &nthis->sd->swlist[src_id];
		nthis->sd->s2slist[i].p_dst_sw = &nthis->sd->swlist[dst_id];
	}
}

void update_net()
{
	nthis->update_all_sw();
	nthis->update_all_s2s();
	nthis->update_all_p2s();
}
/*modified by lix at 2011.9.30*/
//double gethostUsed(int name) 
//{
    //return metricArr[name].used;
	//return  iHost->host.load[name].used;
//}


/*void createlist(int total,int s2s_num,sw2swPtr list,sw_adj_list adj_list)
{
	int i,src,dst,count;
	for(i=0;i<total;i++) adj_list[i].total = 0;
	for(i=0;i<s2s_num;i++)
	{
		src = list[i].sw_src_id;
		dst = list[i].sw_dst_id;
		count = adj_list[src].total++;
		adj_list[src].dst[count] = dst;
		count = adj_list[dst].total++;
		adj_list[dst].dst[count] = src;
	}
}*/
