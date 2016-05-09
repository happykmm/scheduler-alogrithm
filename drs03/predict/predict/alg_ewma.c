/*
  Module: predict module
  Ҫ�󣺸��ݾɵĸ������ݣ�Ԥ��δ���ĸ��� 
  �������һ���ڣ��죩�ĸ��� 
  Created by xianan.
  Created date: 2013/06/14
*/
#include <stdlib.h>
#include <stdio.h>
/***********************************************************************/
/*
* ������ : double predict_ewma(double*data,int data_num,double factor)
* ���� : ��ewma����ǰ����ʱ��㸺��Ԥ����һʱ��㸺��Ԥ��
* ������� : 
				double*data		��ʷ���ݴ������
				int data_num	��ʷ���ݸ���
				double factor	Ȩ��
* ����ֵ :��һʱ���Ԥ��ֵ
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