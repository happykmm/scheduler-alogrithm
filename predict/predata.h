
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
	
	/*实际使用隐层数量*/
	int private_node_num;
	
	/*输入个数*/
	int input_node_num;
	
	/*输出个数*/
	int output_node_num;
	
	/*训练样本数量*/
	int case_num;
	/*[input_node_num][private_node_num]隐藏层权矩阵i,隐层节点最大数量为50*/
	double *private_weight;
	/*[private_node_num][output_node_num]输出层权矩阵*/
	double *output_weight;
	/*输入样本矩阵[case_num][input_node_num]为样本矩阵[case_num+1][input_node_num]的前面case_num个*/
	double *input_array; 
	/*输出样本矩阵[case_num][input_node_num]为样本矩阵[case_num+1][input_node_num]的后面case_num个*/ 
	double *output_array;

	/*[input_node_num]预测结果矩阵*/ 
	double *predict_array;
	
	/*学习速率*/
	double learn_speed; 
	/*精度控制参数*/
	double accuracy;                
	/*最大循环次数*/
	int loop_times;           
} cls_st_bpnn;


#endif