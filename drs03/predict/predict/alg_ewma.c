/*
  Module: predict module
  要求：根据旧的负载数据，预测未来的负载 
  输出：下一周期（天）的负载 
  Created by xianan.
  Created date: 2013/06/14
*/
#include <stdlib.h>
#include <stdio.h>
/***********************************************************************/
/*
* 函数名 : double predict_ewma(double*data,int data_num,double factor)
* 功能 : 用ewma根据前几个时间点负载预测下一时间点负载预测
* 传入参数 : 
				double*data		历史数据存放数组
				int data_num	历史数据个数
				double factor	权重
* 返回值 :下一时间点预测值
* @created by Nan Xia in 2013.8.1
*/

double predict_ewma(double*data,int data_num,double factor)
{ 
	int i;
	double ewma=*data;
	for(i=0;i<data_num;i++)
	{
		ewma=ewma*factor+*(data+i);
	}
	return ewma;
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