#include "../include/kernel.h"
#include "../include/yrws.h"
#include "../include/sh.h"

static void init_yrws(void);
void draw_cursor(void);
void erase_cursor(void);

struct MOUSE_CURSOR cursor;
struct MOUSE_INFO mouse_info;
struct QUEUE *mouse_queue;
struct Layer_Master *LAYER_MASTER;

void yrsw_main(){

      i32_t mouse_buf[512];
      struct Process *me = task_now();
      u32_t data;

      puts("Starting...");

      /*
      *マウスのキューを設定
      */
      mouse_queue = (struct QUEUE *)memory_alloc(memman, sizeof(struct QUEUE));
      queue_init(mouse_queue, 512, mouse_buf, me);

      init_mouse(mouse_queue);
      io_out8(PIC1_IMR, 0xef); // マウスを許可(11101111)

      init_yrws();

      while(1){
            /*
            *マウスのキューはからか?
            */
            if(!queue_size(mouse_queue)){
			/*
			 *割り込み来ないから寝る
			 */
			task_sleep(me);
			io_sti();
            }else{
                  /*
                  *何らかの割り込みが来た
                  */
			data = queue_pop(mouse_queue);
			io_sti();

			if(decode_mdata(data) != 0){
				/*
                        *データが3バイト揃ったので表示
                        */
                        if((mouse_info.button & 0x01) != 0){
					/*
                              *左ボタン
                              */
				}
				if((mouse_info.button & 0x02) != 0){
				      /*
                              *右ボタン
                              */
				}
				if((mouse_info.button & 0x04) != 0){
					/*
                              *中央ボタン
                              */
				}

                        erase_cursor();

				/*
                        *マウスカーソルの移動
                        */
				cursor.x += mouse_info.x;
				cursor.y += mouse_info.y;

                        //Xの限界
				if(cursor.x < 0)
					cursor.x = 0;

                        //Yの限界
				if(cursor.y < 0)
					cursor.y = 0;

                        //逆方向のXの限界
                        if(cursor.x > binfo->scrnx - 1)
					cursor.x = binfo->scrnx - 1;

                        //逆方向のYの限界
				if(cursor.y > binfo->scrny - 1)
					cursor.y = binfo->scrny - 1;

                        draw_cursor();
			}
            }
      }
}

static void init_yrws(void){
      /*
      *レイヤー管理構造体を確保
      */
      layer_master_alloc(LAYER_MASTER);

      struct Layer *wall_paper, *mouse_cursor_layer;
      u8_t *wp_buffer, mc_buffer[128];

      //壁紙のレイヤーを確保
      wall_paper = layer_alloc(LAYER_MASTER);
      //マウスカーソルのレイヤーを確保
      mouse_cursor_layer = layer_alloc(LAYER_MASTER);

      //壁紙のレイヤーの描画情報を格納するバッファを確保
      wp_buffer = (u8_t *)memory_alloc_4k(memman, binfo->scrnx * binfo->scrny);

      boxfill8(binfo->vram, binfo->scrnx, __DEFAULT_WALLPAPER_COLOR__, 0, 0, binfo->scrnx, binfo->scrny);
}
