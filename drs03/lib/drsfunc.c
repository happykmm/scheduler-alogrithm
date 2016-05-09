#include "drsfunc.h"




/*
 * @fn void PM::updateUsed(const VMPtr vm, bool plus)
 * Update pm's used value when add or del a vm
 * @param vm involved vm
 * @param plus means add a vm if set to true
 * @created by tuantuan at 2011.8.20
// */
//void updateUsed(pmPtr pm, vmPtr vm, BOOL plus)
//{
//	int i ;
//	 /*Update used value when add a new vm*/
//    if (plus){     
//       for (i = 0; i < DIM; i++)
//			pm->host.load[i].used +=getCost(vm->host.load,i);
//	}
//    else           // Update used value when del a old vm
//        for (i = 0; i < DIM; i++)
//			pm->host.load[i].used -= getCost(vm->host.load,i);
//
//    return;
//}

/*
 * @fn void PM::resetUsed()
 * Reset all metrics' used value
 * @created by tuantuan at 2011.8.20
 */
void resetUsed(hostPtr ht)
{
	
	int i;
    for (i = 0; i < DIM; i++)
		ht->load[i].used = 0;
    return;
}

/* 
 * @fn void PM::clearVMList()
 * Clear vm list and reset used value
 * @created by tuantuan at 2011.8.20
 */
void clearVMList(pmPtr pm)
{
	int i;
	for(i=0;i<MAX_VM_COUNT_PER_PM;i++){
		pm->vmlist[i].host.id==-1;
		resetUsed(&pm->vmlist[i].host);    
	}
    return;
}



/*
 * @fn void PM::reCalcUsed()
 * Re calculate used value of each metric in PM
 * @created by tuantuan at 2011.8.23
 *//*
void PM::reCalcUsed()
{
	resetUsed();

	VMList::const_iterator it = vmList.begin();

	while (it != vmList.end())
		updateUsed(*it, true);

	return;
}*/

/*
 * @fn bool PM::canFit(const VMPtr vm)
 * Check if a vm can fit on this pm
 * @param vm the vm to be checked
 * @return true if vm can fit on this pm, otherwise return false
 * @create by tuantuan at 2011.8.6
// */
//bool PM::canFit(const VMPtr vm)
//{
//    VMList::const_iterator vit = vmList.begin();
//    MutexSet::const_iterator sit;
//
//    while (vit != vmList.end()) {
//        for (sit = (*vit)->mutexSet.begin(); sit != (*vit)->mutexSet.end(); sit++) {
//            if (*sit == vm->id) {
//                debugMsg("VM %s and %s on pm %s have mutex relationship.\n", vm->getId(), 
//                    (*sit).c_str(), this->getId());
//
//                return false;
//            }
//        }
//
//        vit++;
//    }
//
//    /* Each metric should be satisfied */
//    for (int i = 0; i < DIM; i++) {
//        if (vm->getCost(i) > getAvail(i)) {
//            debugMsg("VM %s need more %s resource than current available in PM %s.\n", 
//                vm->getId(), nameOfMetric[i], this->getId());
//
//            return false;
//        }
//    }
//
//    return true;
//}

/*
 * @fn bool PM::findVM(const VMPtr vm, bool del = false)
 * Check if a pm has hosted specified vm, delete it from this pm's
 * vmlist if found and del is set to true 
 * @param vm vm to be checked
 * @param del delete the vm from pm's vmlist if set to true, false by default
 * @return true if found, otherwise return false
 * @created by tuantuan at 2011.8.20
 */
//bool PM::findVM(const VMPtr vm, bool del)
//{
//    /* Return if vmlist is empty */
//    if (vmList.empty())
//        return false;
//
//    VMList::iterator it = find(vmList.begin(), vmList.end(), vm);
//
//    /* If don't found it */
//    if (it == vmList.end())
//        return false;
//
//    /* If set del to true */
//    if (del) {
//        vmList.erase(it);
//
//        /* Update pm's used value */
//        updateUsed(vm, false);
//    }
//
//    return true;
//}

 /* 
  * @fn void PM::addVM(const VMPtr vm, bool add)
  * Add a vm to pm's vmList
  * @param vm the vm to be added
  * @create by tuantuan at 2011.8.1
  * @edited by lix at 2011.8.8, add check
  * @return true if add successfully, otherwise return false and do nothing
  */
//void PM::addVM(const VMPtr vm, bool add)
//{
//    /* Chnage vm's atPM to this */
//	if (add) {
//    	vm->atPM = this;
//        vm->stat = VMIGR;
//    }
//
//    /* Add this vm to the list */
//    vmList.push_back(vm);
//
//    /* Update pm's used value */
//    updateUsed(vm, true);
//
//    return;
//}

/*
 * @fn void PM::delVM(const VMPtr vm)
 * Delete a vm from pm's vmList
 * @param vm the vm to be deleted
 * @create by tuantuan at 2011.8.1
 */
//void PM::delVM(const VMPtr vm)
//{
//    findVM(vm, true);
//
//    return;
//}

/*
 * @fn VM::VM(const VM &orig)
 * Class VM copy ctor
 */
//VM::VM(const VM &orig): Host(orig), atPM(orig.atPM), mutexSet(orig.mutexSet)
//{ }

/*
void VM::set(int name, int type, double val)
{
    if (!isValid(val))
        return;

    PMPtr pm = this->atPM;

    if (pm == NULL) {
        Host::set(name, type, val);
        return;
    }

    double warm = 0.4;

    switch (type) {
        case USED:
            if (pm->getUsed(name)*warm <= val)
                val = pm->getUsed(name)*warm*0.5;

            metricArr[name].used = val;
            break;
        case TOTAL:
            if (pm->getTotal(name)*warm <= val)
                val = pm->getTotal(name)*warm*0.5;

            metricArr[name].total = val ;
            break;
        case RESERVED:
            metricArr[name].reserved = val;
            break;
        default:
            break;
    }

    return;
}*/

/*
 * @fn bool VM::migrate(PMPtr src, PMPtr dst)
 * Migrate vm from src pm to dst
 * @param src src pm
 * @param dst dst pm
 * @create by tuantuan at 2011.8.1
 */
//bool VM::migrate(PMPtr src, PMPtr dst)
//{
//    /* Do nothing if migrate src and dst is the same */
//    if (src == dst)
//        return true;
//
//    /* If vm can not fit on dst pm */
//    if (!dst->canFit(this))
//        return false;
//
//    dst->addVM(this);        // Add it to dst pm
//    src->delVM(this);        // Delete it from src pm 
//
//    return true;
//}

/*
 * @fn const char *VM::getAt() const 
 * Get the pm id in c string format
 * @created by tuantuan at 2011.8.21
 */
//char  *getAt(vmPtr vm) 
//{
//	pmPtr pm;
//	
//	if (vm->at_pmid != 0)
//        return g_pmlist[vm->at_pmid].host.sid;
//    else
//        return "__NOTATPM__";
//}

/*
 * @fn const char *Request::getReqType() const 
 * Get request type in c string format
 * @created by tuantuan at 2011.8.21
 */
//const char *Request::getReqType() const
//{
//    return requestType.c_str();
//}

/*
 * @fn const char *Request::getAlgName() const 
 * Get algorithm name in c string format
 * @created by tuantuan at 2011.8.21
 */
//const char *Request::getAlgName() const
//{
//    return algorithmName.c_str();
//}

/*
 * @fn const char *Action::getName() const 
 * Get action name in c string format
 * @created by tuantuan at 2011.8.21
 */
//const char *Action::getName() const
//{
//    return name.c_str();
//}

/*
 * @fn const char *Action::getId() const 
 * Get action id in c string format
 * @created by tuantuan at 2011.8.21
 */
//const char *Action::getId() const
//{
//    return id.c_str();
//}

/* 
 * @fn ActionPtr Action::createMigrAct(const std::string &id, const std::string &vmSrcId, 
 *   const std::string &pmSrcIp, const std::string &pmDestIp)
 * Create migrate action instance
 * @param id action id
 * @param vmSrcId the id of vm to be migrated
 * @param pmSrcIp the ip of src pm
 * @param pmDestIp the ip of dest pm
 * @created by tuantuan at 2011.9.13
 */
//ActionPtr Action::createMigrAct(const std::string &id, const std::string &vmSrcId, 
//    const std::string &pmSrcIp, const std::string &pmDestIp)
//{
//    ActionPtr act = new Action("MIGRATE", id);
//
//    act->propMap.insert(std::make_pair("SRC", pmSrcIp));
//    act->propMap.insert(std::make_pair("DST", pmDestIp));
//    act->propMap.insert(std::make_pair("VMID", vmSrcId));
//
//    return act;
//}

/*
 * @fn ActionPtr Action::createDownAct(const std::string &id, const std::string &dstIP)
 * Create down action instance
 * @param id action id
 * @param dstIP the ip of dest pm (that to be shutdown)
 * @created by tuantuan at 2011.9.13
 */
//ActionPtr Action::createDownAct(const std::string &id, const std::string &dstIP)
//{
//    ActionPtr act = new Action("DOWN", id);
//    act->propMap.insert(std::make_pair("TARGET", dstIP));
//
//    return act;
//}

/*
 * @fn ActionPtr Action::createUpAct(const std::string &id, int upNum)
 * Create up action instance
 * @param id action id
 * @param upNum the number of up pms
 * @created by tuantuan at 2011.9.14
 */
//ActionPtr Action::createUpAct(const std::string &id, int upNum)
//{
//    ActionPtr act = new Action("UP", id);
//    act->propMap.insert(std::make_pair("NUM", i2str(upNum)));
//
//    return act;
//}

/*
 * @fn ActionPtr Action::createPlaceAct(const std::string &id, const std::string &vmSrcId, 
 *  const std::string &pmDestIp)
 * Create place action instance
 * @param id action id
 * @param vmSrcId the id of vm to be placed
 * @param dstIP the ip of dest pm (that to be shutdown)
 * @created by tuantuan at 2011.9.14
 */
//ActionPtr Action::createPlaceAct(const std::string &id, const std::string &vmSrcId, 
//    const std::string &pmDestIp)
//{
//    ActionPtr act = new Action("PLACE", id);
//
//    act->propMap.insert(std::make_pair("VMID", vmSrcId));
//    act->propMap.insert(std::make_pair("TARGET", pmDestIp));
//
//    return act;
//}S