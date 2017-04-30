/* FIFOライブラリ */
#include "../include/kernel.h"
#include "../include/value.h"

int queue_push(struct QUEUE *queue, int data);
int queue_push(struct QUEUE *queue, int data);
int queue_size(struct QUEUE *queue);

/*
 *=======================================================================================
 *queue_init関数
 *キューを初期化する関数
 *引数
 *struct QUEUE *fifo
 *=>初期化したいキューへのポインタ
 *int size
 *=>キューに格納できる要素数
 *int *buf
 *=>キューの要素格納に使う配列
 *struct　Process *process
 *=>データが入った時に起こすプロセス
 *返り値
 *int
 *成功すればSUCCESS
 *=======================================================================================
 */
int queue_init(struct QUEUE *queue, int size, int *buf, struct Process *process){

	queue->size = size;       //サイズを指定
	queue->buffer = buf;      //バッファのポインタ
	queue->free = size;       //空き容量(最初はサイズと同じ)
	queue->flags = 0;         //フラグ
	queue->write_index = 0;   //書き込み位置(最初は0)
	queue->read_index = 0;    //読み込み位置(最初は0)
	queue->task = process;	  //データが入った時に起こすプロセス

	/*
	*メンバ関数ポインタを代入
	*/
	queue->push = queue_push;
	queue->pop = queue_pop;
	queue->element_count = queue_size;

	/*
	 *完了
	 */
	return SUCCESS;
}


/*
 *=============================================================================
 *queue_push関数
 *キューにデータをプッシュする関数
 *引数
 *struct QUEUE *fifo
 *=>データをプッシュするキュー
 *int data
 *=>キューにプッシュするデータ
 *=============================================================================
 */
int queue_push(struct QUEUE *queue, int data){
      /*
	 *空きがなくてあふれた
	 */
	if (queue->free == 0) {
		queue->flags |= FLAGS_OVERRUN;
		return FAILURE;
	}

	/*
	 *データをキューにプッシュ
	 */
	queue->buffer[queue->write_index] = data;

	/*
	 *書き込み位置をすすめる
	 */
	queue->write_index++;

	/*
	 *書き込み位置を最初に戻す
	 */
	if (queue->write_index == queue->size) {
		queue->write_index = 0;
	}

	/*
	 *空き容量を一つ分減らす
	 */
	queue->free--;

	/*
	 *タスクを起こす役割をおっていたら
	 */
	if(queue->task != NULL){
		/*
		 *起こすべきタスクが寝ていた
		 */
		if(queue->task->status != RUNNING_PROCESS){
			/*
			 *起こしまーす
			 */
			task_run(queue->task, -1, 0);
		}
	}
	return 0;
}

/*
 *=======================================================================================
 *queue_pop関数
 *引数
 *struct QUEUE *queue
 *=>要素を取り出したいキュー
 *=======================================================================================
 */
int queue_pop(struct QUEUE *queue){	//FIFOからデータを一つとってくる

	/*
	 *値返却用変数
	 */
	int data;

	if (queue->free == queue->size) {
		/*
		 *バッファが空っぽのときは、FAILUREが返される
		 */
		return FAILURE;
	}
	/*
	 *データを取り出す
	 */
	data = queue->buffer[queue->read_index];

	/*
	 *読み込む位置をすすめる
	 */
	queue->read_index++;

	/*
	 *もしも最後まで読んでしまったら0にして最初から読み始める
	 */
	if (queue->read_index == queue->size) {
		queue->read_index = 0;
	}

	/*
	 *値を取り出したので空き容量が一つ増える
	 */
	queue->free++;

	/*
	 *完了
	 */
	return data;
}

/*
 *=======================================================================================
 *queue_size関数
 *キューの現在の要素数を返す関数
 *引数
 *struct QUEUE *queue
 *=>サイズを調べたいキューへのポインタ
 *返り値
 *キューの要素数
 *=======================================================================================
 */
int queue_size(struct QUEUE *queue){
	/*
	 *キューの格納最大数-空き容量=格納数
	 */
	return queue->size - queue->free;
}
