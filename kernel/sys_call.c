#include "../include/bootpack.h"
#include "../include/types.h"
#include "../include/sh.h"
#include "../include/string.h"
#include "../include/yuri.h"

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

/*
 *=======================================================================================
 *do_write関数
 *指定されたストリームに文字列を書き込む関数
 *簡単に言うとwriteシステムコールのOS側の処理
 *引数
 *int fd
 *=>指定するストリーム
 *char *buf
 *=>書き込みたい文字列
 *int count
 *=>書き込む長さ
 *返り値
 *実際に書き込んだ文字列の長さ
 *=======================================================================================
 */
size_t do_write(int fd, char *buf, int count){
	/*
	 *サイズを保持しておく変数
	 */
	int size;

	if(fd == stdout){
		/*
		 *標準出力に書き込む
		 */
		for(size = 0; size < count && buf[size] != '\0'; size++){
			/*
			 *標準出力に書き込む
			 */
			put_char(buf[size]);
		}
	}
	else{
		/*
		 *何もできなかったのでFAILUREを返す1
		 */
		return FAILURE;
	}

	/*
	 *書き込んだ長さを返す
	 */
	return size;
}

/*
 *=======================================================================================
 *do_write関数
 *指定されたストリームから文字列を読み取る関数
 *簡単に言うとreadシステムコールのOS側の処理
 *引数
 *int fd
 *=>指定するストリーム
 *char *buf
 *=>読み取った文字列を格納する変数
 *int count
 *読み取る合計の文字列長
 *返り値
 *実際に読み込んだ文字列の長さ
 *=======================================================================================
 */
size_t do_read(int fd, char *buf, int count){

	if(fd == stdin){
		/*
		 *標準入力から読み取る
		 */
		type_prompt(buf);
	}else{
		/*
		 *何もできなかったのでfailureを返す
		 */
		return FAILURE;
	}

	/*
	 *読み取った文字列長
	 */
	return strlen(buf);
}


int do_get_key(){

	struct Process *me = task_now();
	int i;
	ch_keybuf(&(me->irq));

	for(;;){

		if(!queue_size(&(me->irq))){
			task_sleep(me);
			io_sti();

		}else{
			i = queue_pop(&(me->irq));
			io_sti();
			if(i >= 256 && i <= 511){ //キーボードからの割り込み
				if('A' <= i - 256 && i - 256 <= 'Z'){
					return i;
				}
			}
		}
	}
}
