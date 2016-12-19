#include "../include/kernel.h"
#include "../include/yrws.h"
#include "../include/sh.h"

static void init_yrws(void);
void erase_cursor(void);
void init_mscursor(struct Layer *layer);

struct YRWS_MASTER Yrws_Master;
struct QUEUE *mouse_queue;
struct Layer *mouse_cursor_layer;
struct Layer *wall_paper;
/*
struct MOUSE_CURSOR cursor;
struct Layer_Master *LAYER_MASTER;
*/

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
                        if((Yrws_Master.mouse_info.button & 0x01) != 0){
					/*
                              *左ボタン
                              */
                              draw_window(Yrws_Master.cursor.x, Yrws_Master.cursor.y, 300, 300);
				}
				if((Yrws_Master.mouse_info.button & 0x02) != 0){
				      /*
                              *右ボタン
                              */
				}
				if((Yrws_Master.mouse_info.button & 0x04) != 0){
					/*
                              *中央ボタン
                              */
				}

				/*
                        *マウスカーソルの移動
                        */
				Yrws_Master.cursor.x += Yrws_Master.mouse_info.x;
				Yrws_Master.cursor.y += Yrws_Master.mouse_info.y;

                        //Xの限界
				if(Yrws_Master.cursor.x < 0)
					Yrws_Master.cursor.x = 0;

                        //Yの限界
				if(Yrws_Master.cursor.y < 0)
					Yrws_Master.cursor.y = 0;

                        //逆方向のXの限界
                        if(Yrws_Master.cursor.x > binfo->scrnx - 1)
					Yrws_Master.cursor.x = binfo->scrnx - 1;

                        //逆方向のYの限界
				if(Yrws_Master.cursor.y > binfo->scrny - 1)
					Yrws_Master.cursor.y = binfo->scrny - 1;

                        move_layer(Yrws_Master.LAYER_MASTER, mouse_cursor_layer, Yrws_Master.cursor.x, Yrws_Master.cursor.y);
			}
            }
      }
}

/*
 *=======================================================================================
 *ゆりウィンドウシステムをお管理する構造体を初期化する関数
 *=======================================================================================
 */
static void init_yrws_master(void){
	/*
	 *レイヤー管理構造体を確保
	 */
      layer_master_alloc(&(Yrws_Master.LAYER_MASTER));

      /*
	 *各種パラメータ設定
	 */
	Yrws_Master.screen_height = binfo->scrny;
	Yrws_Master.screen_width = binfo->scrnx;
	Yrws_Master.flags = 0;
	Yrws_Master.video_ram = binfo->vram;

	/*
	 *マウスカーソルの位置
	 */
	Yrws_Master.cursor.x = Yrws_Master.screen_width >> 1;
      Yrws_Master.cursor.y = Yrws_Master.screen_height >> 1;
}

/*
 *=======================================================================================
 *ゆりウィンドウシステムを初期化する関数
 *=======================================================================================
 */
static void init_yrws(void){

      u8_t *wp_buffer, *mc_buffer;

	init_yrws_master();
	
      //壁紙のレイヤーを確保
      wall_paper = layer_alloc(Yrws_Master.LAYER_MASTER);
      //マウスカーソルのレイヤーを確保
      mouse_cursor_layer = layer_alloc(Yrws_Master.LAYER_MASTER);

      //壁紙のレイヤーの描画情報を格納するバッファを確保
      wp_buffer = (u8_t *)memory_alloc_4k(memman, binfo->scrnx * binfo->scrny);
      mc_buffer = (u8_t *)memory_alloc(memman, 128);

      /*
      *壁紙レイヤーの情報をセット
      */
      layer_chbuf(wall_paper, wp_buffer);
      modify_layer(wall_paper, binfo->scrnx, binfo->scrny, -1);
      boxfill8(wall_paper->data, binfo->scrnx, __DEFAULT_WALLPAPER_COLOR__, 0, 0, wall_paper->width, wall_paper->height);

      /*
      *マウスカーソルレイヤーの情報をセット
      */
      layer_chbuf(mouse_cursor_layer, mc_buffer);
      modify_layer(mouse_cursor_layer, 8, 16, 255);
      init_mscursor(mouse_cursor_layer);        //マウスカーソルの描画

      /*
      *レイヤーを移動
      */
      move_layer(Yrws_Master.LAYER_MASTER, wall_paper, 0, 0);
      move_layer(Yrws_Master.LAYER_MASTER, mouse_cursor_layer, Yrws_Master.cursor.x, Yrws_Master.cursor.y);

      /*
      *レイヤーの位置を調整
      */
      layer_ch_position(Yrws_Master.LAYER_MASTER, wall_paper, 0);
      layer_ch_position(Yrws_Master.LAYER_MASTER, mouse_cursor_layer, 1);

      redraw_all_layer(Yrws_Master.LAYER_MASTER, wall_paper, 0, 0, wall_paper->width, wall_paper->height);
}
