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


/*函数功能：从字符串中查找给定的字符，返回字符的位置，否则返回-1
*        返回位置，否则-1
*参数：
*/
int in_str(char *istr,char ch)
{
	int i;
	for(i=0;i<strlen(istr);i++)
	{
		if(istr[i]==ch) return i;
	}
	return -1;
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




/*基于规则从字符串的IDS返回整数的IDS
如：1-3，返回从1到3之间的所有ID    */
int c_sids_to_iids(char *s_ids,int *i_ids)
{
	char *rids[200],*endptr,*rs[30];
	int c,cc,i,j,k,x;
	k=0;
	c=split_str(s_ids," ",rids);

	/*将(1,2,3,5)分割成1 2 3 4数组*/
	
	for(i=0;i<c;i++)
	{
		printf("%s\n",rids[i]);
		x=in_str(rids[i],'-');
		if(x!=-1)/*说明找到了-，属于1-5格式*/
		{
			int b,e;
			cc=split_str(rids[i],"-",rs);
			b=atoi(rs[0]);
			e=atoi(rs[1]);
			for(j=b;j<=e;j++)
				i_ids[k++]=j;
		}
		else
			i_ids[k++]=atoi(rids[i]);
	}
	return k;
}




