#include "util.h"

/*
 * ������ : char* NumToChar(int num)
 * ���� : ��int��������ת����char����
 * ������� : int num
 * ����num : ��Ҫת����int������
 * ����ֵ :char���͵�ָ��
 * @created by �ֿ�ΰ in 2013.5.12
 */
char* NumToChar(int num)
{
	static char str[10];
	sprintf(str,"%d",num);
	return str;
}

/*
 * ������ : char* NumToChar(int num)
 * ���� : ��int��������ת����char����
 * ������� : double num, int n - С��λ��
 * ����num : ��Ҫת����int������
 * ����ֵ :char���͵�ָ��
 * @created by �ֿ�ΰ in 2013.5.12
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



/*�������ܣ��ָ�һ���ַ���
*        ���طָ���ַ����ĸ���
*������
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

