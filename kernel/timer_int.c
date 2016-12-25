#include "../include/kernel.h"

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
