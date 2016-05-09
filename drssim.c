#include "drsctrl.h"

/*for testing*/
int main(){
	
	char xmldir[100]="./input/Data/test02/";

	/*接口1：启动调度模拟系统*/
	start_drs_sys();

	/*接口2：执行调度决策*/
	do_schedule(xmldir,xmldir);

	system("pause");
}
