#include "../../include/types.h"
#include "../../include/sh.h"
#include "../../include/ata.h"
#include "../../include/yrfs.h"

/*
 *=======================================================================================
 *do_sleep関数
 *指定された時間分スリープ状態になるシステムコールのための関数
 *引数
 *unsigned int timeout
 *タイムアウトするまでの時間(ミリ秒で指定する)
 *=======================================================================================
 */
void do_sleep(unsigned int timeout){
	/*
	 *なぜかバグる
	 */
	return;
	//自分自身のプロセスへのポインタを変数に格納

	struct Process *me = task_now();
	/*
	 *自分を起こしてくれるタイマを作る
	 */

	//===============================================ここから
	struct QUEUE queue;                 //目覚まし時計用のキュー
	int buffer[2];                      //キューのバッファ
	/*
	 *キューを初期化
	 *起こすプロセスはもちろん自分
	 */
	queue_init(&queue, 2, buffer, me);

	/*
	 *タイマ変数を宣言&&確保
	 */
	struct TIMER *sleep_timer = timer_alloc();
	/*
	 *タイマを初期化
	 *タイムアウトしたときにデータを流し込むキューはうえで作ったもの
	 */
	timer_init(sleep_timer, &queue, 1);
	//===============================================ここまで

	/*
	 *タイムアウトまでの時間に引数のtimeoutを渡してタイマスタート
	 */
	timer_settime(sleep_timer, timeout);

	/*
	 *タイマの割り込みがくるまで寝る
	 */
	while(1){
		if(!queue_size(&queue)){
			/*
			 *なんもこない
			 */
			task_sleep(me);
			io_sti();
		}else if(queue_pop(&queue) == 1){
			/*
			 *タイマからの割り込みきたー
			 */
			break;
		}
	}

	/*
	 *復帰
	 *タイマを開放して戻るだけ
	 */
	timer_free(sleep_timer);
	return;
}
