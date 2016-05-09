#include "util.h"

/*
 * 函数名 : char* NumToChar(int num)
 * 功能 : 将int类型数据转换成char类型
 * 传入参数 : int num
 * 参数num : 需要转换的int类型数
 * 返回值 :char类型的指针
 * @created by 贾俊伟 in 2013.5.12
 */
char* NumToChar(int num)
{
	static char str[10];
	sprintf(str,"%d",num);
	return str;
}

/*
 * 函数名 : char* NumToChar(int num)
 * 功能 : 将int类型数据转换成char类型
 * 传入参数 : double num, int n - 小数位数
 * 参数num : 需要转换的int类型数
 * 返回值 :char类型的指针
 * @created by 贾俊伟 in 2013.5.12
 */
char* DoubleToChar(double num,int n)
{
	static char str[10];
	sprintf(str,"%.*f" ,n ,num);
	return str;
}


void LogMsg(char *str)
{
	printf("%s",str);	
}

void LogIntoFile(char *str,FILE *fp)
{
	fprintf(fp,"%s\n",str);	
}



/*
 * @fn bool equal(double lhs, double rhs)
 * Check if two values is equal, do approximate compare
 * @param lhs left hand value
 * @param rhs right hand value
 * @return true if two values are approximately equal
 * @created by tuantuan at 2011.8.19
 */
int equal(double lhs, double rhs)
{
    double delta = lhs - rhs;

    if ((delta >= 0 && delta < EPSINON) || (delta < 0 &&delta > -EPSINON))   // modified by wyx at 9.23
        return TRUE;
    else
		return FALSE;
}

/*
 * @fn bool isValid(double val)
 * Check if a value is valid
 * @param val the value to be checked
 * @return true if a value is valid
 * @created by tuantuan at 2011.8.19
 */
int isValid(double val)
{
    return !equal(val, INVI);
}



/*函数功能：分割一个字符串
*        返回分割后字符串的个数
*参数：
*
*/

int split_str(char *istr,char *sfg,char **rets){
	char *p;
	int count=0;
	p = strtok(istr,sfg);
	while(p)
	{
		 //printf("%s\n",p);
		 rets[count++]=p;
		 p=strtok(NULL,sfg);		 
	}
	return count;
}

