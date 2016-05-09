/*
  Module: bpnn predict module
  要求：输入前几周期负载数据输出下一周期负载预测数据 
  输出：下一周期负载预测数据的指针 
  Created by Nan Xia.
  Created date: 2013/07/30
*/
#include "alg_ann.h"

/***********************************************************************/
/*
* 函数名 : void fnet(double net)
* 功能 : 神经网络的信号激励函数
* 传入参数 : 神经元输出信号值1.0/(1.0+exp(-net/10))
* 返回值 :无
* @created by Nan Xia in 2013.7.29
*/

double fnet(double net){ 
	return 1.0/(1.0+exp(-net/10));
}

/***********************************************************************/
/*
* 函数名 : cls_st_bpnn *build(int data_num,int case_number,int learnspeed,int accuracy_in,int looptimes)
* 功能 : 创建一个新的bp神经网络
* 传入参数 : int data_num    每个周期的数据采样个数
             int case_number 样本数量（采样周期数）
			 int learnspeed  学习速率
			 int accuracy_in 控制精度
			 int looptimes   最大循环次数
* 返回值 :返回bp神经网络指针
* @created by Nan Xia in 2013.7.29
*/

cls_st_bpnn *build(int data_num,int case_number,int learnspeed,double accuracy_in,int looptimes){
	/*为新的神经网络分配空间*/
	cls_st_bpnn *THIS=(cls_st_bpnn *)malloc(sizeof(cls_st_bpnn));
	/*实际使用隐层数量*/
	THIS->private_node_num=data_num;
	/*输入个数*/
	THIS->input_node_num=data_num;
	/*输出个数*/
	THIS->output_node_num=data_num;
	/*训练样本数量*/
	THIS->case_num=case_number; 
	/*学习速率*/
	THIS->learn_speed=learnspeed;
	/*精度控制参数*/
	THIS->accuracy=accuracy_in;
	/*最大循环次数*/
	THIS->loop_times=looptimes;
	/*为隐层权重矩阵分配空间*/
	THIS->private_weight=(double*)malloc(sizeof(double)*(THIS->input_node_num+1)*(THIS->private_node_num));
	/*为输出层权重矩阵分配空间*/
	THIS->output_weight=(double*)malloc(sizeof(double)*(THIS->private_node_num+1)*THIS->output_node_num);
	/*为样本输入矩阵分配空间*/
	THIS->input_array=(double*)malloc(sizeof(double)*(case_number+1)*THIS->input_node_num); 
	/*计算样本输出矩阵起始地址*/
	THIS->output_array=THIS->input_array+THIS->input_node_num;
	/*为预测数据分配空间*/
	THIS->predict_array=(double*)malloc(sizeof(double)*THIS->output_node_num);
     
	// int i, j;
	srand((unsigned)time(NULL));
	/*初始化权重矩阵*/
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
* 函数名 : destroy(cls_st_bpnn *THIS)
* 功能 : 销毁神经网络
* 传入参数 : cls_st_bpnn *THIS   bp神经网络指针
* 返回值 :无
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
* 函数名 : tempoutput(cls_st_bpnn *THIS,double*tmppri,double*tmpout,double *pout,double*oout)
* 功能 : 计算各层输出量
* 传入参数 : cls_st_bpnn *THIS   bp神经网络指针
                  double*tmppri         隐层临时权重矩阵指针
			      double*tmpout        输出层临时权重矩阵指针
			      double *pout           隐层输出量矩阵指针
			      double*oout            输出层输出量矩阵指针
* 返回值 :无
* @created by Nan Xia in 2013.7.30
*/

void tempoutput(cls_st_bpnn *THIS,double*tmppri,double*tmpout,double *pout,double*oout) 
{
	//int i,j,k;
	double temp;

	for(k=0; k<THIS->private_node_num; k++){ //计算隐层输出向量
		temp = 0;
		for(j = 0; j < THIS->input_node_num; j++){
			temp = temp + THIS->input_array[i*THIS->input_node_num+j] * tmppri[j*THIS->private_node_num+k];

		}
		temp = temp + THIS->input_array[i*THIS->input_node_num+j] * tmppri[j*THIS->private_node_num+k];             
		pout[k] = fnet(temp);
	}
	for(k = 0; k < THIS->output_node_num; k++){ //计算输出层输出向量
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
* 函数名 : changevalue(cls_st_bpnn *THIS,double*tmppri,double*tmpout,double *pout,double*oout,double *chgp,double*chgo)
* 功能 : 计算各层修改量
* 传入参数 : cls_st_bpnn *THIS   bp神经网络指针
                  double*tmppri         隐层临时权重矩阵指针
			      double*tmpout        输出层临时权重矩阵指针
			      double *pout           隐层输出量矩阵指针
			      double*oout            输出层输出量矩阵指针
				  double *chgp			 隐层修改量矩阵指针
				  double*chgo			 输出层修改量矩阵指针
* 返回值 :无
* @created by Nan Xia in 2013.7.30
*/
void changevalue(cls_st_bpnn *THIS,double*tmppri,double*tmpout,double *pout,double*oout,double *chgp,double*chgo)
{
	// int i,j,k;
	double temp;
	for(j = 0; j < THIS->output_node_num; j++){//计算输出层的权修改量
		chgo[j] = oout[j] * (1-oout[j]) * (THIS->input_array[(i+1)*THIS->input_node_num+j] - oout[j]);       
	}
	for(j = 0; j < THIS->private_node_num; j++){//计算隐层权修改量
		temp = 0;
		for(k = 0; k < THIS->output_node_num; k++){
			temp = temp + tmpout[j*THIS->output_node_num+k] * chgo[k];
		}
		chgp[j] = temp * pout[j] * (1 - pout[j]);
	}
}

/***********************************************************************/
/*
* 函数名 : changeweight(cls_st_bpnn *THIS,double*tmppri,double*tmpout,double *chgp,double*chgo,double*pout)
* 功能 : 计算各层权修改量，并修改临时权矩阵
* 传入参数 : cls_st_bpnn *THIS   bp神经网络指针
                  double*tmppri         隐层临时权重矩阵指针
			      double*tmpout        输出层临时权重矩阵指针
			      double *pout           隐层输出量矩阵指针
				  double*chgo,double *chgp
* 返回值 :无
* @created by Nan Xia in 2013.7.30
*/
void changeweight(cls_st_bpnn *THIS,double*tmppri,double*tmpout,double *chgp,double*chgo,double*pout)
{
	//int i,j,k;
	for(j = 0; j < THIS->private_node_num; j++){//修改输出层权矩阵
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
* 函数名 : train(cls_st_bpnn *THIS)
* 功能 : 训练神经网络
* 传入参数 : cls_st_bpnn *THIS   bp神经网络指针
* 返回值 :无
* @created by Nan Xia in 2013.7.29
* @modified by Nan Xia in 2013.7.30
*/

int train(cls_st_bpnn *THIS)
{
	/*临时权重矩阵*/
	 double *tmppri, *tmpout;
	/*隐层和输出层修改量矩阵*/
	double *chgp, *chgo;
	/*隐层和输出层输出量矩阵*/
	double *pout, *oout; 
	/*temp储存临时数据，e储存误差数据*/
	double e;
	//int i, j, k, n; 
	double duration,start,finish; //记录开始时间和结束时间

	pout=(double*)malloc(sizeof(double)*THIS->private_node_num);//给隐层输出量矩阵分配空间 
	oout=(double*)malloc(sizeof(double)*THIS->output_node_num); //给输出层输出量矩阵分配空间 
	chgp=(double*)malloc(sizeof(double)*(THIS->private_node_num+1));//给隐层修改量矩阵分配空间 
	chgo=(double*)malloc(sizeof(double)*(THIS->output_node_num+1)); //给输出层修改量矩阵分配空间 
	tmppri=(double*)malloc(sizeof(double)*(THIS->input_node_num+1)*THIS->private_node_num);
	tmpout=(double*)malloc(sizeof(double)*(THIS->private_node_num+1)*THIS->output_node_num);
 
	for(i = 0; i < (THIS->input_node_num+1); i++)// 复制结构体中的权矩阵  
		for(j = 0; j < THIS->private_node_num; j++)
		{
			tmppri[i*THIS->private_node_num+j] = THIS->private_weight[i*THIS->private_node_num+j];
		}
         
	for(i = 0; i < (THIS->private_node_num+1); i++)
	for(j = 0; j < THIS->output_node_num; j++)
		tmpout[i*THIS->output_node_num+j] = THIS->output_weight[i*THIS->output_node_num+j];
          

	start=clock();
	for(n=0;/*e>f&&*/n<THIS->loop_times;n++){ //对每个样本训练网络
		e=0;
		for(i= 0; i < THIS->case_num; i++)
		{ 
			tempoutput(THIS,tmppri,tmpout,pout,oout); 
			changevalue(THIS,tmppri,tmpout,pout,oout,chgp,chgo);
			changeweight(THIS,tmppri,tmpout,chgp,chgo,pout);
		}
		/*误差计算*/
		for(i= 0; i < THIS->case_num; i++)
		{
			tempoutput(THIS,tmppri,tmpout,pout,oout);
			for(j = 0; j < THIS->output_node_num ; j++)
			{//计算输出误差
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

	printf("总共循环次数：%d\n", n);
	printf("训练时间：%.2lfs\n",duration);
	for(i = 0; i < THIS->input_node_num; i++) //把结果复制回结构体 
		for(j = 0; j < THIS->private_node_num; j++)
			THIS->private_weight[i*THIS->private_node_num+j] = tmppri[i*THIS->private_node_num+j];
	for(i = 0; i < THIS->private_node_num; i++)
		for (j = 0; j < THIS->output_node_num; j++)
			THIS->output_weight[i*THIS->output_node_num+j] = tmpout[i*THIS->output_node_num+j];
	printf("bp网络训练结束！\n");
	
	return 1;
}

/***********************************************************************/
/*
* 函数名 : resultoutput(cls_st_bpnn *THIS)
* 功能 : 将预测结果输出
* 传入参数 : cls_st_bpnn *THIS   bp神经网络指针
* 返回值 :无
* @created by Nan Xia in 2013.7.30
*/

void resultoutput(cls_st_bpnn *THIS) 
{  
	/*改用XML形式输出*/
	for(i = 0; i < THIS->output_node_num; i++)
	{
		printf("%lf", THIS->predict_array[i]); /*输出结果*/
	}
	printf("\n");
}

/***********************************************************************/
/*
* 函数名 : predict(cls_st_bpnn *THIS)
* 功能 : 预测下一周期
* 传入参数 : cls_st_bpnn *THIS   bp神经网络指针
* 返回值 :无
* @created by Nan Xia in 2013.7.29
*/

int predict(cls_st_bpnn *THIS) 
{  
	
	double *pout; /*隐层输出*/
	double *oout; /*输出层输出*/
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

/*支持从XML读入*/
int fileinput(cls_st_bpnn *THIS)
{
	FILE *fp;
	double tmp;
	//int i,j;
	//char fname[20];
	// printf("请输入文件名");
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


