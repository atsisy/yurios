#include "../../include/kernel.h"
#include "../../include/sh.h"

u32_t load_eip(void);

/*
 *=======================================================================================
 *forkシステムコールの内部関数
 *=======================================================================================
 */
/*
u32_t do_fork(void){

	struct Process *parent = task_now();
	queue_push(&parent->irq, 578);

	struct Process *child;
	child = task_alloc(parent->proc_name);
	i32_t *buf = (i32_t *)memory_alloc(memman, 40);
	queue_init(&child->irq, 10, buf, NULL);

	child->tss.esp = memory_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
	child->tss.es = 1 * 8;
	child->tss.cs = 2 * 8;
	child->tss.ss = 1 * 8;
	child->tss.ds = 1 * 8;
	child->tss.fs = 1 * 8;
	child->tss.gs = 1 * 8;
	child->tss = parent->tss;
	child->sel = parent->sel;
	child->tss.eip = (int)load_eip();

	if(queue_pop(&task_now()->irq) == 875){
		//子プロセスが実行する
		return 0;
	}else{
		//親プロセスが実行する
		queue_push(&child->irq, 875);
		task_run(child, 2, 2);
		return child->pid;
	}
}
*/
