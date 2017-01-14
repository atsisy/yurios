#include "../../../include/kernel.h"
#include "../../../include/yrws.h"
#include "../../../include/sh.h"
#include "../../../include/sysc.h"

extern struct Layer *task_bar;

void draw_clock(i8_t hour, i8_t minute);

/*
 *=======================================================================================
 *タスクバーの時計のプロセス
 *=======================================================================================
 */
void bar_clock_proc(void){
	/*
	 *変数宣言
	 */
	struct Process *me = task_now();
      //確保
	struct Timer_Interrupt *timer_intr = alloc_TimerIntr(me, 875123);
	i8_t hour = do_gettime(__HOUR__), minute = do_gettime(__MINUTE__), itiji = 0;
	char redraw_flag = false;

	//描画
	draw_clock(hour, minute);

	//二十秒おきに時刻を確認する
	TimerIntr_SetTime(timer_intr, 20000);

	while(1){
		if(!queue_size(timer_intr->queue)){
			/*
			 *割り込みは来ていなかった
			 */
			task_sleep(me);
			io_sti();
		}else{
			/*
			 *割り込みが来た
			 */
			queue_pop(timer_intr->queue);
			io_sti();

			/*
			 *時刻が変わっていたら更新
			 */
			if(hour != (itiji = do_gettime(__HOUR__))){
				hour = itiji;
				redraw_flag = true;
			}
			if(minute != (itiji = do_gettime(__MINUTE__))){
				minute = itiji;
				redraw_flag = true;
			}

			if(redraw_flag){
				//描画
				draw_clock(hour, minute);
				redraw_flag = false;
			}

		      /*
			 *タイマ再設定
			 */
			TimerIntr_SetTime(timer_intr, 20000);
		}
	}
}
