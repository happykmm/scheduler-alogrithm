#include "drsctrl.h"

/*for testing*/
int main(){
	
	char xmldir[100]="./input/Data/test01/";

	/*接口2：执行调度决策*/
	do_schedule(xmldir,xmldir);

	system("pause");
}
