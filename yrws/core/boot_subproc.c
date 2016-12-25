#include "../../include/kernel.h"
#include "../../include/yrws.h"

void boot_sub_procs(u32_t flags){
	
	struct YRWS_SUBPROCS *yrws_subs = (struct YRWS_SUBPROCS *)memory_alloc(memman, sizeof(struct YRWS_SUBPROCS));

	if(flags & __TASK_BAR_CLOCK__){
		yrws_subs->bar_clock = task_alloc("taskbarclock");
		yrws_subs->bar_clock->tss.eip = (i32_t)&bar_clock_proc;
		yrws_subs->bar_clock->tss.esp = memory_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
		yrws_subs->bar_clock->tss.es = 1 * 8;
		yrws_subs->bar_clock->tss.cs = 2 * 8;
		yrws_subs->bar_clock->tss.ss = 1 * 8;
		yrws_subs->bar_clock->tss.ds = 1 * 8;
		yrws_subs->bar_clock->tss.fs = 1 * 8;
		yrws_subs->bar_clock->tss.gs = 1 * 8;

		task_run(yrws_subs->bar_clock, 2, 3);
	}
	
}
