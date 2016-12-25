#include "../../../include/kernel.h"
#include "../../../include/yrws.h"
#include "../../../include/sh.h"

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
      //確保
	struct Timer_Interrupt *timer_intr = alloc_TimerIntr(task_now(), 875123);
	i8_t hour = do_gettime(__HOUR__), minute = do_gettime(__MINUTE__), itiji = 0;
	bool redraw_flag = false;

	//描画
	draw_clock(hour, minute);

	//二十秒おきに時刻を確認する
	TimerIntr_SetTime(timer_intr, 20000);

	while(1){
		if(!TimerIntr_came(timer_intr)){
			/*
			 *割り込みは来ていなかった
			 */
			io_stihlt();
		}else{
			/*
			 *割り込みが来た
			 */

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

void draw_clock(i8_t hour, i8_t minute){
	char time[5];
	zeroclear_8array(time, 5);
	sprintf(time, "%d:%d", hour, minute);
	boxfill8(task_bar->data, task_bar->width, __RGB256COL__(36, 49, 61), task_bar->width-48, 0, task_bar->width, 16);
	putfonts8_asc(task_bar->data, task_bar->width, task_bar->width-48, 0, __RGB256COL__(255, 255, 255), time);
	redraw_all_layer(Yrws_Master.LAYER_MASTER, task_bar, 0, 0, task_bar->width, 16);
}
