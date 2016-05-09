/*
  Module: bpnn predict module
  Ҫ������ǰ�����ڸ������������һ���ڸ���Ԥ������ 
  �������һ���ڸ���Ԥ�����ݵ�ָ�� 
  Created by Nan Xia.
  Created date: 2013/07/30
*/
#include "alg_ann.h"

/***********************************************************************/
/*
* ������ : void fnet(double net)
* ���� : ��������źż�������
* ������� : ��Ԫ����ź�ֵ1.0/(1.0+exp(-net/10))
* ����ֵ :��
* @created by Nan Xia in 2013.7.29
*/

double fnet(double net){ 
	return 1.0/(1.0+exp(-net/10));
}

/***********************************************************************/
/*
* ������ : cls_st_bpnn *build(int data_num,int case_number,int learnspeed,int accuracy_in,int looptimes)
* ���� : ����һ���µ�bp������
* ������� : int data_num    ÿ�����ڵ����ݲ�������
             int case_number ����������������������
			 int learnspeed  ѧϰ����
			 int accuracy_in ���ƾ���
			 int looptimes   ���ѭ������
* ����ֵ :����bp������ָ��
* @created by Nan Xia in 2013.7.29
*/

cls_st_bpnn *build(int data_num,int case_number,int learnspeed,double accuracy_in,int looptimes){
	/*Ϊ�µ����������ռ�*/
	cls_st_bpnn *THIS=(cls_st_bpnn *)malloc(sizeof(cls_st_bpnn));
	/*ʵ��ʹ����������*/
	THIS->private_node_num=data_num;
	/*�������*/
	THIS->input_node_num=data_num;
	/*�������*/
	THIS->output_node_num=data_num;
	/*ѵ����������*/
	THIS->case_num=case_number; 
	/*ѧϰ����*/
	THIS->learn_speed=learnspeed;
	/*���ȿ��Ʋ���*/
	THIS->accuracy=accuracy_in;
	/*���ѭ������*/
	THIS->loop_times=looptimes;
	/*Ϊ����Ȩ�ؾ������ռ�*/
	THIS->private_weight=(double*)malloc(sizeof(double)*(THIS->input_node_num+1)*(THIS->private_node_num));
	/*Ϊ�����Ȩ�ؾ������ռ�*/
	THIS->output_weight=(double*)malloc(sizeof(double)*(THIS->private_node_num+1)*THIS->output_node_num);
	/*Ϊ��������������ռ�*/
	THIS->input_array=(double*)malloc(sizeof(double)*(case_number+1)*THIS->input_node_num); 
	/*�����������������ʼ��ַ*/
	THIS->output_array=THIS->input_array+THIS->input_node_num;
	/*ΪԤ�����ݷ���ռ�*/
	THIS->predict_array=(double*)malloc(sizeof(double)*THIS->output_node_num);
     
	// int i, j;
	srand((unsigned)time(NULL));
	/*��ʼ��Ȩ�ؾ���*/
	for(i = 0; i < THIS->input_node_num+1; i++){
		for(j = 0; j < THIS->private_node_num; j++){
			THIS->private_weight[i*THIS->private_node_num+j] = rand() / (double)(RAND_MAX);
		}
	}
	for(i = 0; i < THIS->private_node_num+1; i++){
		for(j = 0; j < THIS->output_node_num; j++){
			THIS->output_weight[i*THIS->output_node_num+j] = rand() / (double)(RAND_MAX);
		}
	}
	return THIS;
}

/***********************************************************************/
/*
* ������ : destroy(cls_st_bpnn *THIS)
* ���� : ����������
* ������� : cls_st_bpnn *THIS   bp������ָ��
* ����ֵ :��
* @created by Nan Xia in 2013.7.29
*/
void destroy(cls_st_bpnn *THIS){
	free(THIS->private_weight);
	free(THIS->output_weight);
	free(THIS->input_array);
	free(THIS->predict_array);
	free(THIS); 
} 

/***********************************************************************/
/*
* ������ : tempoutput(cls_st_bpnn *THIS,double*tmppri,double*tmpout,double *pout,double*oout)
* ���� : ������������
* ������� : cls_st_bpnn *THIS   bp������ָ��
                  double*tmppri         ������ʱȨ�ؾ���ָ��
			      double*tmpout        �������ʱȨ�ؾ���ָ��
			      double *pout           �������������ָ��
			      double*oout            ��������������ָ��
* ����ֵ :��
* @created by Nan Xia in 2013.7.30
*/

void tempoutput(cls_st_bpnn *THIS,double*tmppri,double*tmpout,double *pout,double*oout) 
{
	//int i,j,k;
	double temp;

	for(k=0; k<THIS->private_node_num; k++){ //���������������
		temp = 0;
		for(j = 0; j < THIS->input_node_num; j++){
			temp = temp + THIS->input_array[i*THIS->input_node_num+j] * tmppri[j*THIS->private_node_num+k];

		}
		temp = temp + THIS->input_array[i*THIS->input_node_num+j] * tmppri[j*THIS->private_node_num+k];             
		pout[k] = fnet(temp);
	}
	for(k = 0; k < THIS->output_node_num; k++){ //����������������
		temp = 0;
		for(j = 0; j < THIS->private_node_num; j++){
			temp = temp + pout[j] * tmpout[j*THIS->output_node_num+k];
		}
		temp = temp + pout[j] * tmpout[j*THIS->output_node_num+k];
		oout[k] = fnet(temp); 
		printf("%lf",oout[k]);

	}
	printf("\n");
}

/***********************************************************************/
/*
* ������ : changevalue(cls_st_bpnn *THIS,double*tmppri,double*tmpout,double *pout,double*oout,double *chgp,double*chgo)
* ���� : ��������޸���
* ������� : cls_st_bpnn *THIS   bp������ָ��
                  double*tmppri         ������ʱȨ�ؾ���ָ��
			      double*tmpout        �������ʱȨ�ؾ���ָ��
			      double *pout           �������������ָ��
			      double*oout            ��������������ָ��
				  double *chgp			 �����޸�������ָ��
				  double*chgo			 ������޸�������ָ��
* ����ֵ :��
* @created by Nan Xia in 2013.7.30
*/
void changevalue(cls_st_bpnn *THIS,double*tmppri,double*tmpout,double *pout,double*oout,double *chgp,double*chgo)
{
	// int i,j,k;
	double temp;
	for(j = 0; j < THIS->output_node_num; j++){//����������Ȩ�޸���
		chgo[j] = oout[j] * (1-oout[j]) * (THIS->input_array[(i+1)*THIS->input_node_num+j] - oout[j]);       
	}
	for(j = 0; j < THIS->private_node_num; j++){//��������Ȩ�޸���
		temp = 0;
		for(k = 0; k < THIS->output_node_num; k++){
			temp = temp + tmpout[j*THIS->output_node_num+k] * chgo[k];
		}
		chgp[j] = temp * pout[j] * (1 - pout[j]);
	}
}

/***********************************************************************/
/*
* ������ : changeweight(cls_st_bpnn *THIS,double*tmppri,double*tmpout,double *chgp,double*chgo,double*pout)
* ���� : �������Ȩ�޸��������޸���ʱȨ����
* ������� : cls_st_bpnn *THIS   bp������ָ��
                  double*tmppri         ������ʱȨ�ؾ���ָ��
			      double*tmpout        �������ʱȨ�ؾ���ָ��
			      double *pout           �������������ָ��
				  double*chgo,double *chgp
* ����ֵ :��
* @created by Nan Xia in 2013.7.30
*/
void changeweight(cls_st_bpnn *THIS,double*tmppri,double*tmpout,double *chgp,double*chgo,double*pout)
{
	//int i,j,k;
	for(j = 0; j < THIS->private_node_num; j++){//�޸������Ȩ����
		for(k = 0; k < THIS->output_node_num; k++){
			tmpout[j*THIS->output_node_num+k] = tmpout[j*THIS->output_node_num+k] + THIS->learn_speed * pout[j] * chgo[k];
		}
	}
	for(j = 0; j < THIS->input_node_num; j++){
		for(k = 0; k < THIS->private_node_num; k++){
			tmppri[j*THIS->private_node_num+k] = tmppri[j*THIS->private_node_num+k] + THIS->learn_speed * THIS->input_array[i*THIS->input_node_num+j] * chgp[k];
		}
	}
}

/***********************************************************************/
/*
* ������ : train(cls_st_bpnn *THIS)
* ���� : ѵ��������
* ������� : cls_st_bpnn *THIS   bp������ָ��
* ����ֵ :��
* @created by Nan Xia in 2013.7.29
* @modified by Nan Xia in 2013.7.30
*/

int train(cls_st_bpnn *THIS)
{
	/*��ʱȨ�ؾ���*/
	 double *tmppri, *tmpout;
	/*�����������޸�������*/
	double *chgp, *chgo;
	/*�������������������*/
	double *pout, *oout; 
	/*temp������ʱ���ݣ�e�����������*/
	double e;
	//int i, j, k, n; 
	double duration,start,finish; //��¼��ʼʱ��ͽ���ʱ��

	pout=(double*)malloc(sizeof(double)*THIS->private_node_num);//������������������ռ� 
	oout=(double*)malloc(sizeof(double)*THIS->output_node_num); //�������������������ռ� 
	chgp=(double*)malloc(sizeof(double)*(THIS->private_node_num+1));//�������޸����������ռ� 
	chgo=(double*)malloc(sizeof(double)*(THIS->output_node_num+1)); //��������޸����������ռ� 
	tmppri=(double*)malloc(sizeof(double)*(THIS->input_node_num+1)*THIS->private_node_num);
	tmpout=(double*)malloc(sizeof(double)*(THIS->private_node_num+1)*THIS->output_node_num);
 
	for(i = 0; i < (THIS->input_node_num+1); i++)// ���ƽṹ���е�Ȩ����  
		for(j = 0; j < THIS->private_node_num; j++)
		{
			tmppri[i*THIS->private_node_num+j] = THIS->private_weight[i*THIS->private_node_num+j];
		}
         
	for(i = 0; i < (THIS->private_node_num+1); i++)
	for(j = 0; j < THIS->output_node_num; j++)
		tmpout[i*THIS->output_node_num+j] = THIS->output_weight[i*THIS->output_node_num+j];
          

	start=clock();
	for(n=0;/*e>f&&*/n<THIS->loop_times;n++){ //��ÿ������ѵ������
		e=0;
		for(i= 0; i < THIS->case_num; i++)
		{ 
			tempoutput(THIS,tmppri,tmpout,pout,oout); 
			changevalue(THIS,tmppri,tmpout,pout,oout,chgp,chgo);
			changeweight(THIS,tmppri,tmpout,chgp,chgo,pout);
		}
		/*������*/
		for(i= 0; i < THIS->case_num; i++)
		{
			tempoutput(THIS,tmppri,tmpout,pout,oout);
			for(j = 0; j < THIS->output_node_num ; j++)
			{//����������
				//e=e + (THIS->input_array[i+1][j] - oout[j]) * (THIS->input_array[i+1][j] - O2[j]);
				if((THIS->input_array[(i+1)*THIS->input_node_num+j]*0.8)<oout[j]
					&&oout[j]<(THIS->input_array[(i+1)*THIS->input_node_num+j]*1.2))
				{
					e++;
				}
			}        
		}
	}
	finish=clock();

	duration=(double)(finish-start)/CLOCKS_PER_SEC;
	printf("%.2lf%%\n",e/THIS->input_node_num/THIS->case_num*100);

	printf("�ܹ�ѭ��������%d\n", n);
	printf("ѵ��ʱ�䣺%.2lfs\n",duration);
	for(i = 0; i < THIS->input_node_num; i++) //�ѽ�����ƻؽṹ�� 
		for(j = 0; j < THIS->private_node_num; j++)
			THIS->private_weight[i*THIS->private_node_num+j] = tmppri[i*THIS->private_node_num+j];
	for(i = 0; i < THIS->private_node_num; i++)
		for (j = 0; j < THIS->output_node_num; j++)
			THIS->output_weight[i*THIS->output_node_num+j] = tmpout[i*THIS->output_node_num+j];
	printf("bp����ѵ��������\n");
	
	return 1;
}

/***********************************************************************/
/*
* ������ : resultoutput(cls_st_bpnn *THIS)
* ���� : ��Ԥ�������
* ������� : cls_st_bpnn *THIS   bp������ָ��
* ����ֵ :��
* @created by Nan Xia in 2013.7.30
*/

void resultoutput(cls_st_bpnn *THIS) 
{  
	/*����XML��ʽ���*/
	for(i = 0; i < THIS->output_node_num; i++)
	{
		printf("%lf", THIS->predict_array[i]); /*������*/
	}
	printf("\n");
}

/***********************************************************************/
/*
* ������ : predict(cls_st_bpnn *THIS)
* ���� : Ԥ����һ����
* ������� : cls_st_bpnn *THIS   bp������ָ��
* ����ֵ :��
* @created by Nan Xia in 2013.7.29
*/

int predict(cls_st_bpnn *THIS) 
{  
	
	double *pout; /*�������*/
	double *oout; /*��������*/
	double temp;
	//int i;

	pout = (double*)malloc(sizeof(double)*THIS->private_node_num);
	oout = (double*)malloc(sizeof(double)*THIS->output_node_num);

	i=THIS->case_num;
	tempoutput(THIS,THIS->private_weight,THIS->output_weight,pout,oout);
	for(i = 0; i < THIS->output_node_num; i++)
	{
		THIS->predict_array[i]=oout[i];
	}
	
	
	printf("\n");

	return 1;
}

/*֧�ִ�XML����*/
int fileinput(cls_st_bpnn *THIS)
{
	FILE *fp;
	double tmp;
	//int i,j;
	//char fname[20];
	// printf("�������ļ���");
	//scanf("%s",fname);
	if((fp=fopen("./predict/datann.txt","r"))==NULL)
	{
		printf("error!");
		return 1;
	}
	else
	{
		for(i=0;(i<THIS->case_num+1)&&(feof(fp)==0);i++)
		{
			for(j=0;(j<THIS->input_node_num)&&(feof(fp)==0);j++)
			{
				fscanf(fp,"%lf",&tmp);
				THIS->input_array[i*THIS->input_node_num+j]=tmp/100000;
			}
		}
	return 0;
	}
}


