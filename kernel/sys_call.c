#include "../include/kernel.h"
#include "../include/types.h"
#include "../include/sh.h"
#include "../include/ata.h"
#include "../include/yrfs.h"
#include "../include/util_macro.h"


int do_get_key(){

	struct Process *me = task_now();
	int i;
	ch_keybuf(me->irq);

	for(;;){

		if(IS_FAILURE(queue_size(me->irq))){
			task_sleep(me);
			io_sti();

		}else{
			i = queue_pop(me->irq);
			io_sti();
			if(i >= 256 && i <= 511){ //キーボードからの割り込み
				if('A' <= i - 256 && i - 256 <= 'Z'){
					return i;
				}
			}
		}
	}
}
