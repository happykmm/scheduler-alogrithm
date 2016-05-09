/*
  Module: bpnn predict module
  Ҫ������ǰ�����ڸ������������һ���ڸ���Ԥ������ 
  �������һ���ڸ���Ԥ�����ݵ�ָ�� 
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
* ������ : void init_dataprocess(cls_dataprocessPtr THIS)
* ���� : ��ʼ�����ݴ�����ĳ�Ա����
* ������� : cls_dataprocessPtr THIS ���ݴ�����ָ��
* ����ֵ :��
* @created by Nan Xia in 2013.7.29
*/


int main()
{
//	int i,j;
    int tmp;
	cls_st_bpnn *bp;

	/*�ⲿ����*/

	bp=build(41,6,1,0.1,100); 

	printf("�ֶ������밴0���ļ������밴1\n");
	scanf("%d",&tmp);
	if(tmp==0)
	{ 

	//  directinput(bp.input_array,bp.output_array);
                
	}
	else
	{
		
		tmp=fileinput(bp); /*��������*/

		if(tmp==1){return 1;}
	}
	if(PDEBUG)
	{
		printf("����:\n");//�����ʷ����
		for(i=0;i< bp->case_num;i++)
		{
			for(j=0;j<bp->input_node_num;j++)
			{
				printf("%lf ",bp->input_array[i*bp->input_node_num+j]);
			}
			printf("\n");
		}
	
		printf("Ŀ�����:\n");//���Ŀ������
		
		/*�����XML*/
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
	
	
	train(bp);    //ѵ��bp������

	predict(bp);  //����bp������
	
	
	resultoutput(bp);/*������*/


	getchar();
	getchar();
	
	return 1;
}

