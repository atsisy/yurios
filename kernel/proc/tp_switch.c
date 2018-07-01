#include "../../include/mm.h"

extern struct PROCESS_MASTER *process_master;
extern struct TIMER *task_timer;
/*
 *=======================================================================================
 *task_switchsub関数
 *タスク切換えのサブルーチン
 *=======================================================================================
 */
void task_switchsub(void){
	int i;
	//一番上のレベルを探す
	for(i = 0;i < MAX_TASKLEVELS;i++){
		if(process_master->PROCESS_APARTMENT[i].THIS_FLOOR_PROCESSES > 0){
			break;	//見つかった
		}
	}
	process_master->now_process_level = i;
	process_master->level_change_flag = 0;
	return;
}

/*
 *=======================================================================================
 *task_switch関数
 *実行するタスクを入れ替える関数
 *
 *=======================================================================================
 */
void task_switch(void){
	/*
	 *現在実行中のプロセスのフロアの配列を取得
	 */
	struct PROCESS_LEVEL_FLOOR *floor = &process_master->PROCESS_APARTMENT[process_master->now_process_level];

	/*
	 *現在実行中のプロセスを取得
	 *
	 */
	struct Process *new_task, *now_task = floor->ROOMS[floor->NOW_RUNNING_PROCESS];

      /*
	 *実行中のプロセスの部屋をお隣の部屋に変更
	 */
	floor->NOW_RUNNING_PROCESS++;
	/*
	 *もしもお隣のへやは誰もいなかったら
	 */
	if(floor->NOW_RUNNING_PROCESS == floor->THIS_FLOOR_PROCESSES){
		floor->NOW_RUNNING_PROCESS = 0;
	}

	/*
	 *もしも実行するプロセスのフロアを変えるよう命令が来ていたらフロアを変更
	 */
	if(process_master->level_change_flag){
		task_switchsub();
		floor = &process_master->PROCESS_APARTMENT[process_master->now_process_level];
	}

	/*
	 *新しく実行されるタスクを確定
	 */
	new_task = floor->ROOMS[floor->NOW_RUNNING_PROCESS];

      /*
	 *プロセスのタイムアウトまでの時間を設定
	 */
	timer_settime(task_timer, new_task->priority);

	/*
	 *新しいプロセスに飛ぶ
	 */
	if (new_task != now_task) {
		farjmp(0, new_task->sel);
	}

	/*
	 *完了
	 */
	return;
}
