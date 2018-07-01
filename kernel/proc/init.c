#include "../../include/mm.h"

struct PROCESS_MASTER *process_master;
struct TIMER *task_timer;

void task_idle(void);

/*
 *=======================================================================================
 *プロセスのマンションの初期化
 *建築（笑）
 *=======================================================================================
 */
struct Process *task_init(struct MEMMAN *memman, char *p_name){
	int i;
	struct Process *task, *idle;

	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
	/*
	 *プロセスマスター（マンション本体兼管理人（笑））を確保
	 */
	process_master = (struct PROCESS_MASTER *)memory_alloc_4k(memman, sizeof(struct PROCESS_MASTER));
	process_master->top_pid = 0;

	/*
	 *マンションの入居者名簿（フロアは関係ない）を初期化
	 */
	for(i = 0;i < MAX_TASKS;i++){
		/*
		 *誰もいませんよ
		 */
		process_master->ALL_PROCESSES[i].status = NULL_PROCESS;
		process_master->ALL_PROCESSES[i].sel = (TASK_GDT + i) << 3;	//8をかけたい
		set_segmdesc(gdt + TASK_GDT + i, 103, (int)&process_master->ALL_PROCESSES[i].tss, AR_TSS32);
	}
	/*
	 *フロアごとに初期化
	 */
	for(i = 0;i < MAX_TASKLEVELS;i++){
		process_master->PROCESS_APARTMENT[i].THIS_FLOOR_PROCESSES = 0;
		process_master->PROCESS_APARTMENT[i].NOW_RUNNING_PROCESS = 0;
	}
	
	/*
	 *カーネルのプロセス
	 */
	task = task_alloc(p_name);
	task->status = RUNNING_PROCESS;	//動作中のマーク
	task->priority = 2;
	task->level = 0;
	task_add(task);
	task->pid = issue_pid();
	task_switchsub();
	load_tr(task->sel);
	task_timer = timer_alloc();
	timer_settime(task_timer, task->priority);

	/*
	 *アイドルタスク（番兵役）を確保
	 */
	idle = task_alloc("idle process");
	idle->tss.esp = memory_alloc_4k(memman, 64*1024) + 64 * 1024;
	idle->tss.eip = (int)&task_idle;
	idle->tss.es = 1 * 8;
	idle->tss.cs = 2 * 8;
	idle->tss.ss = 1 * 8;
	idle->tss.ds = 1 * 8;
	idle->tss.fs = 1 * 8;
	idle->tss.gs = 1 * 8;
	idle->pid = issue_pid();
	/*
	 *GOGO
	 */
	task_run(idle, MAX_TASKLEVELS-1, 1);

	return task;
}

/*
 *=======================================================================================
 *task_idle関数
 *プロセスキューの番兵プロセス用のプロセスのための処理
 *見ればわかるようにHLTをし続ける
 *=======================================================================================
 */
void task_idle(void){
	while(1){
		io_hlt();
	}
}
