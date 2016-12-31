#include "../../include/yurixx.hxx"

class Queue {
	
	i32_t *buffer;
	i32_t write_index;
	i32_t read_index;
	i32_t size;
	i32_t free;
	i32_t flags;
	struct Process *wproc;

public:
	Queue(i32_t size, i32_t *buffer, struct Process *proc){
		this->size = size;
		this->buffer = buffer;
		this->wproc = proc;
		this->free = size;
		this->flags = 0;
		this->write_index = 0;
		this->read_index = 0;
	}

	i32_t push(i32_t data){
		/*
		 *空きがなくてあふれた
		 */
		if(free == 0){
			flags |= FLAGS_OVERRUN;
			return -1;
		}

		/*
		 *キューにデータをプッシュ
		 */
		buffer[write_index] = data;

		/*
		 *書き込み位置をすすめる
		 */
		write_index++;

		/*
		 *もしも書き込み位置がオーバーフローしていたら
		 */
		if(write_index == size){
			//元に戻す
			write_index = 0;
		}

		/*
		 *空き容量をデクリメント
		 */
		free--;

		/*
		 *タスクを起こさなければならない場合
		 */
		if(wproc != NULL){
			/*
			 *目的のプロセスは寝ているか
			 */
			if(wproc->status != RUNNING_PROCESS){
				/*
				 *プロセスを起こす
				 */
				task_run(wproc, -1, 0);
			}
		}

		return data;
	}

	i32_t pop(){
		i32_t data;

		/*
		 *バッファが空っぽのとき
		 */
		if(free == size){
			/*
			 *-1を返す
			 */
			return -1;
		}

		/*
		 *データを取り出す
		 */
		data = buffer[read_index];

		/*
		 *読み込み位置をすすめる
		 */
		read_index++;

		/*
		 *オーバーフローしていたら
		 */
		if(read_index == size){
			//元に戻す
			read_index = 0;
		}

		/*
		 *空き容量を増やす
		 */
		free++;

		/*
		 *データを返す
		 */
		return data;
	}

	i32_t remain(){
		return size - free;
	}

	bool isEmpty(){
		return !free;
	}
};
