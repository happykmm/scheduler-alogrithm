#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#include "predata.h"


int i,j,k,n;/*全局计数变量*/




double fnet(double net);

cls_st_bpnn *build(int data_num,int case_number,int learnspeed,double accuracy_in,int looptimes);

void destroy(cls_st_bpnn *THIS);

void tempoutput(cls_st_bpnn *THIS,double*tmppri,double*tmpout,double *pout,double*oout);

void changevalue(cls_st_bpnn *THIS,double*tmppri,double*tmpout,double *pout,double*oout,double *chgp,double*chgo);

void changeweight(cls_st_bpnn *THIS,double*tmppri,double*tmpout,double *chgp,double*chgo,double*pout);

int train(cls_st_bpnn *THIS);

void resultoutput(cls_st_bpnn *THIS) ;

int predict(cls_st_bpnn *THIS);

int fileinput(cls_st_bpnn *THIS); 
