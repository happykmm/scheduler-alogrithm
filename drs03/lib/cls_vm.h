#ifndef CLS_VM_H
#define CLS_VM_H


#include "drsdata.h"
#include "drsdef.h"
#include "drslib.h"
#include "util.h"

typedef struct {
	st_vm vm;	
	st_share_data *sd;

	//void (*get_n_aff_fr_set)();
	//int (*get_aff_fr_set)(int vm_id,int set[]);
} cls_st_vm;
typedef cls_st_vm *clsstvmPtr;

clsstvmPtr vthis;
void initVm(clsstvmPtr THIS);
//void get_n_aff_fr_set();
//int get_aff_fr_set(int vm_id,int set[]);
#endif