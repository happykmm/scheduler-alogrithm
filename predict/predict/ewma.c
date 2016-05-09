/*
  Module: predict module
  Ҫ�󣺸��ݾɵĸ������ݣ�Ԥ��δ���ĸ��� 
  �������һ���ڣ��죩�ĸ��� 
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
@�������ܣ����½�һ�������ʱ�򴴽�һ���µĶ��󣬲��ҷ��ض���ĵ�ַ 
����˵����time_point_num һ�������еĸ������ݸ�����fact��ewma��Ȩ������ 
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
@�������ܣ�����������ر�ʱ���ٶ��� 
����˵����THISΪָ��Ŀ������ָ�� 
*/
void destroy(cls_st_ewma *THIS){//free the memory of chosen struct and the data array
     free(THIS->data);
     free(THIS); 
    } 
     
/*
@�������ܣ��õ�һ�����ڣ��죩�����ݳ�ʼ������ 
����˵����THIS ָ��Ŀ������ָ�룬data_first_day[]�����һ�����ڵ����ݵ����� 
*/
void initialize(cls_st_ewma *THIS,double data_first_day[]){//use the data of the first day to initialize the data array
    int i;
    for(i=0;i<THIS->tpn;i++)
       {
        THIS->data[i]=data_first_day[i];
       }
   }
/*
@�������ܣ����¶��󣬽�֮ǰÿ������ݵ�Ȩ�س���1-Ȩ�������ټ�����һ������ݳ������� 
����˵����THIS ָ��Ŀ������ָ�룬data_today[]�����������ݵ����� 
*/
void update(cls_st_ewma *THIS,double data_today[]){//update run when need  
    int i;
    for(i=0;i<THIS->tpn;i++){
        THIS->data[i]=THIS->data[i]*(1-THIS->factor)+data_today[i]*THIS->factor;
    }
}
/*
@�������ܣ�Ԥ��ģ�飬�����һ���ڣ��죩�ĸ������� 
����˵����THIS ָ��Ŀ������ָ��
*/
void predict(cls_st_ewma *THIS){//
    int i;
    for(i=0;i<THIS->tpn;i++){
        printf("%lf ",THIS->data[i]);
    }
 //return &(THIS->data);
}
/*
@�������ܣ��õ�һ�����ڣ��죩�����ݳ�ʼ������ 
����˵����THIS ָ��Ŀ������ָ�룬data_first_day[]�����һ�����ڵ����� 
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
