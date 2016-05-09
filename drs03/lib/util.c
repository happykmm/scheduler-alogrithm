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


/*�������ܣ����ַ����в��Ҹ������ַ��������ַ���λ�ã����򷵻�-1
*        ����λ�ã�����-1
*������
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




/*���ڹ�����ַ�����IDS����������IDS
�磺1-3�����ش�1��3֮�������ID    */
int c_sids_to_iids(char *s_ids,int *i_ids)
{
	char *rids[200],*endptr,*rs[30];
	int c,cc,i,j,k,x;
	k=0;
	c=split_str(s_ids," ",rids);

	/*��(1,2,3,5)�ָ��1 2 3 4����*/
	
	for(i=0;i<c;i++)
	{
		printf("%s\n",rids[i]);
		x=in_str(rids[i],'-');
		if(x!=-1)/*˵���ҵ���-������1-5��ʽ*/
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




