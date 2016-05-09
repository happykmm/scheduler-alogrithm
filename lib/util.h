#ifndef _DRS_UTIL_H
#define _DRS_UTIL_H
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define BOOL int
#define MAXDELAY 32767
#define INVI -1
#define EPSINON 0.00001

char* NumToChar(int num);/*将int类型数据转换成char类型*/

void LogMsg(char *str);

void LogIntoFile(char *str,FILE *fp);

int equal(double lhs, double rhs);

int isValid(double val);

/*分割一个字符串*/
int split_str(char *instring, char seps[], char *ret_s[]);

int c_sids_to_iids(char *s_ids,int *i_ids);
#endif
