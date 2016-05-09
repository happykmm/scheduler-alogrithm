/*
  Module: bpnn predict module
  要求：输入前几周期负载数据输出下一周期负载预测数据 
  输出：下一周期负载预测数据的指针 
  Created by Nan Xia.
  Created date: 2013/07/30
*/
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#include "alg_ann.h"

#include "pred_ctrl.h"

/***********************************************************************/
/*
* 函数名 : void init_dataprocess(cls_dataprocessPtr THIS)
* 功能 : 初始化数据处理类的成员变量
* 传入参数 : cls_dataprocessPtr THIS 数据处理类指针
* 返回值 :无
* @created by Nan Xia in 2013.7.29
*/


int main()
{
//	int i,j;
    int tmp;
	cls_st_bpnn *bp;

	/*外部输入*/

	bp=build(41,6,1,0.1,100); 

	printf("手动输入请按0，文件导入请按1\n");
	scanf("%d",&tmp);
	if(tmp==0)
	{ 

	//  directinput(bp.input_array,bp.output_array);
                
	}
	else
	{
		
		tmp=fileinput(bp); /*读入数据*/

		if(tmp==1){return 1;}
	}
	if(PDEBUG)
	{
		printf("输入:\n");//输出历史数据
		for(i=0;i< bp->case_num;i++)
		{
			for(j=0;j<bp->input_node_num;j++)
			{
				printf("%lf ",bp->input_array[i*bp->input_node_num+j]);
			}
			printf("\n");
		}
	
		printf("目标输出:\n");//输出目标数据
		
		/*输出到XML*/
		for(i=0;i<bp->case_num;i++)
		{
			for(j=0;j<bp->input_node_num;j++)
			{
				printf("%lf ",bp->output_array[i*bp->input_node_num+j]);
			}
			printf("\n");
		}
	}

	getchar();
	getchar();
	
	
	train(bp);    //训练bp神经网络

	predict(bp);  //测试bp神经网络
	
	
	resultoutput(bp);/*输出结果*/


	getchar();
	getchar();
	
	return 1;
}

