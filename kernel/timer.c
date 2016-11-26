#include "../include/bootpack.h"
#include "../include/value.h"

struct TIMER_MASTER timer_master;

/*
 *=======================================================================================
 *init_pit関数
 *PITを初期化する関数
 *1秒間に1000回割り込みが来るように設定する
 *=>1msに一回
 *=======================================================================================
 */
void init_pit(void){

	int i;
	struct TIMER *t;

	/*
	 *1msに一回タイマ割り込みが来るように設定する
	 */
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0xa9);
	io_out8(PIT_CNT0, 0x04);

	/*
	 *起動からの時間を0にセット
	 */
	timer_master.count = 0;

	/*
	 *タイマの配列を初期化
	 */
	for(i = 0;i < MAX_TIMER;i++){
		/*
		*未使用のタイマフラグを立てる
		*/
		timer_master.ALL_TIMERS[i].status = NULL_TIMER;
	}

	/*
	 *番兵用のタイマを一つ確保
	 */
	t = timer_alloc();
	t->timeout = 0xffffffff;        //タイムアウトまでの時間は最大に
	t->status = USING_TIMER;    //番兵用のタイマの状態をUSINGに
	t->next = NULL;		        //一番うしろだから後ろはNULL
	timer_master.proximate_timer = t;	  //今は番兵しかいないので先頭でもある
	timer_master.proximate_timeout = 0xffffffff;	  //番兵の時刻

	return;
}

/*
 *=======================================================================================
 *inthandler20関数
 *タイマ割り込みのイベントハンドラ関数
 *タイマのカウントをしたりタイムアップ処理などを行う
 *=======================================================================================
 */
void inthandler20(int *esp){

	io_out8(PIC0_OCW2, 0x60);	//IRQ-00受付完了をPICに通知

	/*
	 *起動してからの時間をインクリメント
	 */
	timer_master.count++;

	/*
	 *もし次にタイムアウトするタイマはまだタイムアウトしないなら
	 */
	if(timer_master.proximate_timeout > timer_master.count){
		/*
		 *何もせず終了
		 */
		return;
	}

	/*
	 *ここまで到達したということは
	 *次にタイムアウトするタイマがタイムアウトするということ
	 */

	char ts = 0;
	struct TIMER *timer;

	/*
	 *timer変数に次にタイムアウトするタイマへのポインタを代入
	 */
	timer = timer_master.proximate_timer;

	while(1){
		/*
		 *次のタイマはまだタイムアウトしないなら
		 */
		if(timer->timeout > timer_master.count){
			/*
			 *ループから脱出
			 */
			break;
		}

		/*
		 *ここに到達したということはタイムアウトした
		 */
		timer->status = ALLOC_TIMER;
		if(timer != task_timer){
			queue_push(timer->fifo, timer->data);
		}else{
			/*
			 *タスクスケジューラのタイマがタイムアウトした
			 */
			ts = 1;  //フラグ建築
		}
		timer = timer->next;
	}

	/*
	 *タイマを次のタイマに設定
	 *タイムアウト時間も新しいタイマのタイムアウト時間に更新
	 */
	timer_master.proximate_timer = timer;
	timer_master.proximate_timeout = timer_master.proximate_timer->timeout;

	if(ts != 0){
		/*
		 *タスクを切り替えてあげます
		 */
		task_switch();
	}

	/*
	 *無事終了
	 */
	return;
}

/*
 *=======================================================================================
 *timer_alloc関数
 *タイマを確保する関数
 *返り値
 *新しく確保したタイマへのポインタ
 *=======================================================================================
 */
struct TIMER *timer_alloc(void){

	int i;

	/*
	 *未使用中のタイマは無いか探す(NULL_TIMER)
	 */
	for(i = 0;i < MAX_TIMER;i++){
		/*
		 *NULL_TIMERかどうか
		 */
		if(timer_master.ALL_TIMERS[i].status == NULL_TIMER){
			/*
			 *未使用のタイマを発見
			 */
			timer_master.ALL_TIMERS[i].status = ALLOC_TIMER;
			/*
			 *未使用のタイマの状態を変更し、そのポインタを返し終了
			 */
			return &timer_master.ALL_TIMERS[i];
		}
	}

	/*
	 *見つからなかった
	 */
	return FAILURE;
}

/*
 *=======================================================================================
 *timer_free関数
 *タイマを開放する関数
 *引数
 *struct TIMER *timer
 *=>開放したいタイマへのポインタ
 *=======================================================================================
 */
void timer_free(struct TIMER *timer){
	/*
	 *特に動的確保をしているわけではないので状態をNULL_TIMERにして終了
	 */
	timer->status = NULL_TIMER;
	/*
	 *無事終了
	 */
	return;
}

/*
 *=======================================================================================
 *timer_init関数
 *タイマを初期化する関数
 *引数
 *struct TIMER *timer
 *=>初期化したいタイマへのポインタ
 *struct QUEUE *queue
 *=>タイムアウトしたときにデータを流し込みたいキューへのポインタ
 *unsigned char data
 *=>タイムアウトしたときに指定したキューに流し込みたい値
 *=======================================================================================
 */
void timer_init(struct TIMER *timer, struct QUEUE *queue, unsigned char data){
	/*
	 *そのまんまですね
	 */
	timer->fifo = queue;
	timer->data = data;
	/*
	 *無事終了
	 */
	return;
}

/*
 *=======================================================================================
 *timer_settime関数
 *タイマの時間設定する関数
 *この関数が終了と同時にカウントダウンが始まる
 *引数
 *struct TIMER *timer
 *=>時間設定をしたいタイマへのポインタ
 *unsigned int timeout
 *設定したい時間(ミリ秒で指定する)
 *=======================================================================================
 */
void timer_settime(struct TIMER *timer, unsigned int timeout){

	int e;
	struct TIMER *forward, *s;

	/*
	 *タイムアウト時間を設定
	 */
	timer->timeout = timeout + timer_master.count;
	/*
	 *タイマの状態もUSINGに変更
	 */
	timer->status = USING_TIMER;

	e = io_load_eflags();
	io_cli();

	/*
	 *forwardには次にタイムアウトするタイマへのポインタを代入
	 */
	forward = timer_master.proximate_timer;

      /*
	 *もしもタイムアウトまでの時間が今あるタイマの中で一番短かったら
	 */
	if(timer->timeout <= forward->timeout){
		/*
		*先頭に入れることになる
		*/
		timer_master.proximate_timer = timer;               //先頭のタイマへのポインタに代入
		timer->next = forward;					    //次はいままで先頭だったタイマになる
		timer_master.proximate_timeout = timer->timeout;    //次のタイムアウトまでの時間も更新
		io_store_eflags(e);
		return;
	}

	/*
	 *どこに入れればいいか探す
	 */
	while(1){
		s = forward;                  //forwardを一回保持
		forward = forward->next;      //forwardの次を見に行く
		if(timer->timeout <= forward->timeout){   //もしもタイムアウトまでの時間が次のタイマのタイムアウトより早かったら
			s->next = timer;	      //sの次はtimer
			timer->next = forward;	//timerの次はforward
			io_store_eflags(e);
			return;
		}
	}
}
