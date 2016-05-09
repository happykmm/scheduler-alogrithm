
/*
 * @file predata.h includes data types, data structures and definitions
 * @created by xianan <3110102814@zju.edu.cn>, zhejiang university and huwei l.t.d
 * @created by xianan at 2013.8.1
 */

#ifndef PREDATA_H_INCLUDED
#define PREDATA_H_INCLUDED

#define ACCURACY 1
#define LEARNSPEED 0.1
#define MAXLOOPTIMES 100

#define PDEBUG 1


typedef struct { 
	
	/*ʵ��ʹ����������*/
	int private_node_num;
	
	/*�������*/
	int input_node_num;
	
	/*�������*/
	int output_node_num;
	
	/*ѵ����������*/
	int case_num;
	/*[input_node_num][private_node_num]���ز�Ȩ����i,����ڵ��������Ϊ50*/
	double *private_weight;
	/*[private_node_num][output_node_num]�����Ȩ����*/
	double *output_weight;
	/*������������[case_num][input_node_num]Ϊ��������[case_num+1][input_node_num]��ǰ��case_num��*/
	double *input_array; 
	/*�����������[case_num][input_node_num]Ϊ��������[case_num+1][input_node_num]�ĺ���case_num��*/ 
	double *output_array;

	/*[input_node_num]Ԥ��������*/ 
	double *predict_array;
	
	/*ѧϰ����*/
	double learn_speed; 
	/*���ȿ��Ʋ���*/
	double accuracy;                
	/*���ѭ������*/
	int loop_times;           
} cls_st_bpnn;


#endif