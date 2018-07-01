#include <mm.h>

/*
 *=======================================================================================
 *alloc_TimerIntr関数
 *Timer_Interrupt構造体を確保する関数
 *引数
 *struct Process *proc
 *=>タイマが切れたら起こすプロセス
 *i32_t identifier
 *=>タイマ割り込みのときに割り込まれる値
 *返り値
 *確保したTimer_Interrupt構造体へのポインタ
 *=======================================================================================
 */
struct Timer_Interrupt *alloc_TimerIntr(struct Process *proc, i32_t identifier){
	//Timer_Interrupt構造体を確保
	struct Timer_Interrupt *timer_intr = (struct Timer_Interrupt *)memory_alloc(memman, sizeof(struct Timer_Interrupt));

	//キューを初期化
	timer_intr->queue = (struct QUEUE *)memory_alloc(memman, sizeof(struct QUEUE));
	queue_init(timer_intr->queue, __TIMER_INTERRUPT_BUFFER_SIZE__, timer_intr->intr_buf, proc);

	//タイマのセットアップ
	timer_intr->timer = timer_alloc();
	timer_init(timer_intr->timer, timer_intr->queue, identifier);

	return timer_intr;
}

/*
 *=======================================================================================
 *free_TimerIntr関数
 *確保したTimer_Interrupt構造体を開放する関数
 *引数
 *struct Timer_Interrupt *timer_intr
 *=>開放したいTimer_Interrupt構造体
 *=======================================================================================
 */
void free_TimerIntr(struct Timer_Interrupt *timer_intr){

	//キューを開放
	memory_free(memman, (u32_t)timer_intr->queue, sizeof(struct QUEUE));

	//タイマを開放
	timer_free(timer_intr->timer);

	//Timer_Interruptを開放
	memory_free(memman, (u32_t)timer_intr, sizeof(struct Timer_Interrupt));

}

/*
 *=======================================================================================
 *TimerIntr_SetTime関数
 *Timer_Interrupt構造体のタイマに時間をセットする関数
 *引数
 *struct Timer_Interrupt *timer_intr
 *=>時間をセットしたいTimer_Interrupt構造体
 *i32_t time_limit
 *タイムリミット
 *返り値
 *セットした時間
 *=======================================================================================
 */
i32_t TimerIntr_SetTime(struct Timer_Interrupt *timer_intr, i32_t time_limit){

	//タイムリミットを設定
	timer_settime(timer_intr->timer, time_limit);

	return time_limit;
}

/*
 *=======================================================================================
 *TimerIntr_came関数
 *割り込みが来ているかどうか判定する関数
 *=======================================================================================
 */
char TimerIntr_came(struct Timer_Interrupt *timer_intr){
	return queue_size(timer_intr->queue);
}
