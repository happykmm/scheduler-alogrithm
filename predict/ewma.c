/*
  Module: predict module
  要求：根据旧的负载数据，预测未来的负载 
  输出：下一周期（天）的负载 
  Created by xianan.
  Created date: 2013/06/14
*/
#include <stdlib.h>
#include <stdio.h>

typedef struct{
    int tpn;       //time_point_number ,the number of time point in a cycle
    double factor; //the power
    double *data;  //the pointer of the array stores calculated data
 }cls_st_ewma;
/*
@函数功能：当新建一个虚拟机时则创建一个新的对象，并且返回对象的地址 
参数说明：time_point_num 一个周期中的负载数据个数，fact是ewma的权重因子 
*/
cls_st_ewma *build(int time_point_num,double fact){// parameters: time_point_number and factor
     cls_st_ewma* tmp;                             
     tmp=(cls_st_ewma*)malloc(sizeof(cls_st_ewma));//allocate memory for the struct
     tmp->tpn=time_point_num;                      //
     tmp->factor=fact;                             //
     tmp->data=(double*)malloc(sizeof(double)*time_point_num);//allocate memory for the data array
     return tmp;//return the pointer of the struct
    }
/*
@函数功能：当虚拟机被关闭时销毁对象 
参数说明：THIS为指向目标对象的指针 
*/
void destroy(cls_st_ewma *THIS){//free the memory of chosen struct and the data array
     free(THIS->data);
     free(THIS); 
    } 
     
/*
@函数功能：用第一个周期（天）的数据初始化对象 
参数说明：THIS 指向目标对象的指针，data_first_day[]储存第一个周期的数据的数组 
*/
void initialize(cls_st_ewma *THIS,double data_first_day[]){//use the data of the first day to initialize the data array
    int i;
    for(i=0;i<THIS->tpn;i++)
       {
        THIS->data[i]=data_first_day[i];
       }
   }
/*
@函数功能：更新对象，将之前每天的数据的权重乘上1-权重因子再加上新一天的数据乘上因子 
参数说明：THIS 指向目标对象的指针，data_today[]储存今天的数据的数组 
*/
void update(cls_st_ewma *THIS,double data_today[]){//update run when need  
    int i;
    for(i=0;i<THIS->tpn;i++){
        THIS->data[i]=THIS->data[i]*(1-THIS->factor)+data_today[i]*THIS->factor;
    }
}
/*
@函数功能：预测模块，输出下一周期（天）的负载数据 
参数说明：THIS 指向目标对象的指针
*/
void predict(cls_st_ewma *THIS){//
    int i;
    for(i=0;i<THIS->tpn;i++){
        printf("%lf ",THIS->data[i]);
    }
 //return &(THIS->data);
}
/*
@函数功能：用第一个周期（天）的数据初始化对象 
参数说明：THIS 指向目标对象的指针，data_first_day[]储存第一个周期的数据 
*/
int fileinput(double x[6][50]){//input data from file  
    FILE *fp;
    double tmp;
    int i,j;
    if((fp=fopen("data.txt","r"))==NULL){
        printf("error!");
        return 1;
        }
    else{
        for(i=0;(i<6)&&(feof(fp)==0);i++){
                for(j=0;(j<41)&&(feof(fp)==0);j++){
                     fscanf(fp,"%lf",&tmp);
                     x[i][j]=tmp;
                    }
                }
                return 0;
        }
}
int main()
{ 
    int tpn,i;
    cls_st_ewma *ewma;
    double x[6][50];
    ewma=build(41,0.5);
    fileinput(x);  
    initialize(ewma,x[0]);
    for(i=0;i<6;i++){
        update(ewma,x[i]);
        predict(ewma);
        printf("\n");
       }
getchar();
}
