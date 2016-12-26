#include "../../include/kernel.h"
#include "../../include/limits.h"
#include "../../include/string.h"
#include "../../include/value.h"
#include "../../include/sh.h"

extern struct PROCESS_MASTER *process_master;
extern struct TIMER *task_timer;

/*
 *=======================================================================================
 *task_now関数
 *現在実行されているプロセスへのポインタを返す関数
 *返り値
 *現在実行されているプロセスへのポインタ
 *=======================================================================================
 */
struct Process *task_now(void){
	/*
	 *1.現在動作中のプロセスレベルを取得
	 *2.プロセスのマンションに取得した階層を指定してその階層を変数に格納
	 */
	struct PROCESS_LEVEL_FLOOR *floor = &process_master->PROCESS_APARTMENT[process_master->now_process_level];
	/*
	 *取得した階層内の実行中のプロセスへのポインタを返す
	 */
	return floor->ROOMS[floor->NOW_RUNNING_PROCESS];
}

/*
 *=======================================================================================
 *task_add関数
 *プロセスをマンションに入居させる関数
 *引数
 *struct Process *task
 *=>入居させるプロセスへのポインタ
 *=======================================================================================
 */
void task_add(struct Process *task){
	/*
	 *入居したいフロアを聞いて、そのフロアの配列を取得
	 */
	struct PROCESS_LEVEL_FLOOR *floor = &process_master->PROCESS_APARTMENT[task->level];
	/*
	 *目的のフロアの一番近い部屋に入居させる0
	 *(誰かが退去したとき、全員スライドさせて詰めるので以下のコードでOK)
	 */
	floor->ROOMS[floor->THIS_FLOOR_PROCESSES] = task;
	/*
	 *入居者が増えたことを反映
	 */
	floor->THIS_FLOOR_PROCESSES++;
	/*
	 *入居者のステータスを変更
	 */
	task->status = RUNNING_PROCESS;

	return;
	//あとで100超えた場合の処理を追加(入居者がオーバーしたときのこと)
}


/*
 *=======================================================================================
 *task_alloc関数
 *新しくプロセスを確保する関数
 *引数
 *char *p_name
 *=>新しく確保するプロセスのプロセス名
 *=======================================================================================
 */
struct Process *task_alloc(char *p_name){

	int i;
	/*
	 *新しいプロセス
	 */
	struct Process *new_process;
	/*
	 *プロセスマンションの入居者名簿に記入(フロアはまだ関係ない)
	 */
	for (i = 0; i < MAX_TASKS; i++){
		/*
		 *プロセスマンションの名簿にあいているところはありますか？
		 */
		if (process_master->ALL_PROCESSES[i].status == NULL_PROCESS){
			/*
			 *あいている場所があった
			 */

			/*
			 *名簿に記入
			 *マンションに入居できたことを知らせる（変数に名簿に載った場所のポインタを代入）
			 */
			new_process = &process_master->ALL_PROCESSES[i];

			/*
			 *プロセスに名前をつける
			 */
			strcpy(new_process->proc_name, p_name);

			/*
			 *プロセスを確保
			 *状態はスリープ状態
			 */
			new_process->status = SLEEPING_PROCESS;
			new_process->tss.eflags = 0x00000202; /* IF = 1; */
			new_process->tss.eax = 0; /* とりあえず0にしておくことにする */
			new_process->tss.ecx = 0;
			new_process->tss.edx = 0;
			new_process->tss.ebx = 0;
			new_process->tss.ebp = 0;
			new_process->tss.esi = 0;
			new_process->tss.edi = 0;
			new_process->tss.es = 0;
			new_process->tss.ds = 0;
			new_process->tss.fs = 0;
			new_process->tss.gs = 0;
			new_process->tss.idtr = 0;
			new_process->tss.iomap = 0x40000000;

			new_process->pid = issue_pid();
			

			/*
			 *新しく入居できたプロセスへのポインタを返す
			 */
			return new_process;
		}
	}

	/*
	 *マンションはいっぱいでした
	 */
	return FAILURE;
}

/*
 *=======================================================================================
 *task_remove関数
 *指定したプロセスをマンションから退去させる関数
 *引数
 *struct Process *process
 *=>退去させるプロセス
 *=======================================================================================
 */
void task_remove(struct Process *process){

	int i;

	/*
	 *退去したいプロセスが入居しているフロアを取得し、そのフロアの配列を取得
	 */
	struct PROCESS_LEVEL_FLOOR *floor = &process_master->PROCESS_APARTMENT[process->level];

	/*
	 *退去したがっているプロセスの部屋を特定する
	 */
	for (i = 0; i < floor->THIS_FLOOR_PROCESSES; i++) {
		if (floor->ROOMS[i] == process) {
			/*
			 *指定されたプロセスの部屋を発見
			 */
			break;
		}
	}

	/*
	 *プロセスが退去するフロアの入居人数を一つ分減らす
	 */
	floor->THIS_FLOOR_PROCESSES--;

	/*
	 *もしも今動作しているプロセスが今回退去することになるプロセスよりも奥の部屋に住んでいると
	 *退去したあと部屋をずらしたときにどこにいるかわからなくなるので先に合わせておきたい
	 */
	if (i < floor->NOW_RUNNING_PROCESS) {
		floor->NOW_RUNNING_PROCESS--; //ずれるからここも合わせる
	}

	/*
	 *もしも今動作しているプロセスが一番奥に住んでると狂うので修正したい
	 */
	if (floor->NOW_RUNNING_PROCESS >= floor->THIS_FLOOR_PROCESSES) {
		//NOW_RUNNING_PROCESSが狂っていたら修正
		floor->NOW_RUNNING_PROCESS = 0;
	}

	/*
	 *退去したあとRUNNING状態のままもおかしいのでスリープ状態にする
	 */
	process->status = SLEEPING_PROCESS;

	/*
	 *部屋を退去した分ずらしまーす
	 */
	for (; i < floor->THIS_FLOOR_PROCESSES; i++) {
		/*
		 *一つ手前の部屋に引越し引越し
		 */
		floor->ROOMS[i] = floor->ROOMS[i + 1];
	}

	/*
	 *退去完了
	 */
	return;
}

/*
 *=======================================================================================
 *task_run関数
 *プロセスを実行または寝ているプロセスを再実行させる関数
 *引数
 *struct Process *process
 *=>実行したいプロセス
 *int level
 *=>実行させたいプロセスレベル
 *int priority
 *=>実行させたい優先度
 *=======================================================================================
 */
void task_run(struct Process *process, int level, int priority){

	static int pid_master;

	/*
	 *レベルにマイナスを指定した場合、レベルは今の状態のままになる
	 */
	if(level < 0){
		level = process->level;	//レベルは今のまま
	}

	/*
	 *優先度にマイナスを指定した場合、優先度は今の状態のままになる
	 */
	if(priority > 0){
		process->priority = priority;   //優先度は今のまま
	}

	/*
	 *現在動作中で動作レベルを変更したい場合
	 */
	if(process->status == RUNNING_PROCESS && process->level != level){
		/*
		 *一回マンションから退去
		 *ステータスはSLEEPING_PROCESSになる
		 */
		task_remove(process);	//一時的にスリープするけど下の処理ですぐに起こされる(flagsは1になる)
	}

	/*
	 *もしプロセスが寝ていたら
	 */
	if(process->status != RUNNING_PROCESS){
		/*
		 *スリープから起こされる
		 *task_add関数内でRUNNING状態にされるやろ
		 */
		process->level = level;
		task_add(process);
	}
	process_master->level_change_flag = 1;	//次回のタスクスイッチの時にレベルを見直す

	/*
	 *プロセスIDを付与
	 */
	process->pid = pid_master;

	/*
	 *次のプロセスのためにインクリメント
	 */
	pid_master++;

	/*
	 *完了
	 */
	return;
}

/*
 *=======================================================================================
 *task_sleep関数
 *プロセスをスリープ状態にする関数
 *引数
 *struct Process *task
 *=>スリープ状態にしたいプロセスへのポインタ
 *=======================================================================================
 */
void task_sleep(struct Process *task){

	struct Process *now_task;

	/*
	 *もし指定タスクがもし起きていたら
	 *=>タスクがすでに寝ていたら何もしない
	 */
	if(task->status == RUNNING_PROCESS){
		now_task = task_now();
		/*
		 *タスクを退去させて実行が回ってこないようにする
		 */
		task_remove(task);
		if(task == now_task){
			//自分自身のスリープなのでタスクスイッチが必要
			task_switchsub();
			now_task = task_now();
			farjmp(0, now_task->sel);
		}
	}

	/*
	 *完了
	 */
	return;
}

/*
 *=======================================================================================
 *issue_pid関数
 *プロセスIDを発行する関数
 *=======================================================================================
 */
u32_t issue_pid(){
	process_master->top_pid++;
	return process_master->top_pid - 1;
}
