/*
 * @file cls_host.c includes host definitions functions to call publicly
 * 
 * @created by jianhaichen <chenjh919@zju.edu.cn>, zhejiang university and huwei l.t.d
 * @modified by jianhaichen at 2013.4.12
 */

#include "cls_host.h"


void initHost(clshostPtr THIS)
{
	iHost=THIS;	

	THIS->getAvail=getAvail;
	THIS->getTotal=getTotal;
	THIS->getUsed=getUsed;
	THIS->getReserved=getReserved;
	THIS->getName=getName;
	THIS->getSId=getSId;
	THIS->get=get;
	THIS->set=set;
	THIS->getpmbysid=getpmbysid;
	THIS->getvmbysid=getvmbysid;
	
		
}


double getTotal(int name)
{
    return iHost->host.load[name].total;
}


/*modified by lix at 2011.9.30*/
double getUsed(int name) 
{
    //return metricArr[name].used;
	return  iHost->host.load[name].used;;
}

double getReserved(int name) 
{
    //return metricArr[name].used;
	return  iHost->host.load[name].reserved;
}



double getAvail(int name) 
{
	//return metricArr[name].total - metricArr[name].reserved
		//- getUsed(name);
	return getTotal(name) - getUsed(name)-getReserved(name);
}



/*
 * @fn const char *Metric::getName() const 
 * Get metric name in c string format
 * @created by tuantuan at 2011.8.21
 */
char *getName(int name)
{
    return nameOfMetric[name];
}


/*
 * @fn double Host::set(int name, int type, double val)
 * Set value of the specified metric name and type
 * @param name metric name
 * @param type metric type
 * @param val the actual value
 * @created by lix at 2011.7.31
 * @modified by tuantuan at 2011.8.16
 */
void set(int name, int type, double val)
{
    if (!isValid(val))
        return;

    switch (type){
        case USED:
			iHost->host.load[name].used = val;
            break;
        case TOTAL:
            iHost->host.load[name].total = val ;
            break;
        case RESERVED:
            iHost->host.load[name].reserved = val;
            break;
        default:
            break;
    }

    return;
}

/*
 * @fn double Host::get(int name, int type)
 * Get the value of specified metric name and type
 * @param name metric name
 * @param type metric type
 * @return the value of specified metric name and type
 * @created by lix at 2011.7.31
 * @modified by tuantuan at 2011.8.16
 */
double get(int name, int type)
{
    double val;

    switch (type) {
        case USED:
            val = getUsed(name);
            break;
        case TOTAL:
			val = getTotal(name);
            break;
        case RESERVED:
            val = getReserved(name);
            break;
        case AVAIL:
            val = getAvail(name);
            break;
        default:
            val = INVI;
    }

    return val;
}

/*
 * @fn const char *Host::getId() const 
 * Get host id in c string format
 * @created by tuantuan at 2011.8.21
 */
char *getSId() 
{
	return iHost->host.sid;
}

/*
 * @fn const char *Host::getIp() const 
 * Get host ip in c string format
 * @created by tuantuan at 2011.8.21
 */
char *getIp(hostPtr hst)
{
	return iHost->host.ip;
}


/*
 * @fn pmPtr getpmbysid(char *sid) 
 * para: char *sid
 * @created by Jianhai Chen at 2013.7.18
 */
pmPtr getpmbysid(char *sid)
{
	int i;
	pmPtr ipm=NULL;
	if(strlen(sid)==0) return NULL;
	for(i=0;i<iHost->sd->pm_total;i++)
	{
		if(strcmp(sid,iHost->sd->pmlist[i].host.sid)==0){
			//找到了
			ipm=&iHost->sd->pmlist[i];
			break;
		}
	}
	
	return ipm;
}



/*
 * @fn vmPtr getvmbysid(char *sid) 
 * para: char *sid
 * return: 返回获取的vm
 * @created by Jianhai Chen at 2013.7.18
 */
vmPtr getvmbysid(char *sid)
{
	int i;
	vmPtr ivm=NULL;
	for(i=0;i<iHost->sd->vm_total;i++)
	{
		if(strcmp(sid,iHost->sd->vmlist[i].host.sid)==0){
			//找到了
			ivm=&iHost->sd->vmlist[i];
			break;
		}
	}
	
	return ivm;
}
