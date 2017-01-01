#include "../../include/kernel.h"

void end(void){
	struct Process *proc = task_now();
	while(1){
		task_sleep(proc);
	}
}
