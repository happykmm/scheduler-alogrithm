#include "drsctrl.h"

/*for testing*/
int main(){
	
	char xmldir[100]="./input/Data/test02/";

	/*�ӿ�1����������ģ��ϵͳ*/
	start_drs_sys();

	/*�ӿ�2��ִ�е��Ⱦ���*/
	do_schedule(xmldir,xmldir);

	system("pause");
}
