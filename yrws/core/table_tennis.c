#include "../../include/yrws.h"
#include "../../include/kernel.h"
#include "../../include/sh.h"

char isAlphabet(char code);

char KeyTable[26];

/*
 *キーボードの情報
 *キーを話した時は、キーコードに0x80を足した値が帰ってくる
 */

int tt_main(){
	boxfill8(binfo->vram, binfo->scrnx, 0, 40, 40, 300, 300);
	i32_t i;
	struct QUEUE *queue = (struct QUEUE *)memory_alloc(memman, sizeof(struct QUEUE));
	struct Process *me = task_now();

	struct YURI_WINDOW *window = create_window("tennis", 50, 50, 600, 600);

	struct TIMER *timer = timer_alloc();
	/*
	 *タイムリミットになったら10が割り込んでくる
	 */
	timer_init(timer, queue, 10);

	i32_t *key_buffer = (i32_t *)memory_alloc(memman, sizeof(i32_t) * 128);
	queue_init(queue, 128, key_buffer, NULL);
	ch_keybuf(queue);

	timer_settime(timer, 1000 / 60);

	while(1){
		if(!queue_size(queue)){
			//キューが空っぽだから寝る
			task_sleep(me);
			io_sti();
		}else{
			i = queue_pop(queue);
			/*
			 *キーボードからの割り込み
			 */
			if(i >= 256 && i <= 511){
				if(isAlphabet(keys0[i] - 0x41)){
					/*
					 *キーを押し込んだ
					 */
					KeyTable[keys0[i] - 0x41] = 1;
				}else if(isAlphabet(keys0[i - 0x80] - 0x41)){
					/*
					 *キー離した
					 */
					KeyTable[keys0[i - 0x80] - 0x41] = 0;
				}
			}else if(i == 10){
				if(KeyTable['A' - 0x41]){
					puttext(window->layer, "a pushed", 20, 20, 20);
				}

				timer_settime(timer, 1000 / 60);
			}
		}
	}
}

char isAlphabet(char code){
	return (code >= 0 && code <= 25) ? 1 : 0;
}
