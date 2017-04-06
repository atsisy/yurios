#include "../../include/kernel.h"
#include "../../include/yrws.h"
#include "../../include/sh.h"
#include "../../include/util_macro.h"

static void init_yrws(void);
void erase_cursor(void);
i32_t fae(i32_t function, u32_t argc, char *command, u32_t flag);
void init_mscursor(struct Layer *layer);
int tt_main();

struct YRWS_MASTER Yrws_Master;
struct QUEUE *mouse_queue;
struct Layer *mouse_cursor_layer;
struct Layer *wall_paper;
struct Layer *task_bar;
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

	/*
	struct YURI_WINDOW *window = create_window("Image View", 200, 200, 500, 500);

	struct YURI_IMAGE *image = load_yim("yuri.yim");

	if(image != NULL){
		draw_yim(window, image, 10, 10);
	}
	*/

	//fae((u32_t)tt_main, 0, NULL, 0);

	struct Process *child = task_alloc("aaaaa");
	child->tss.esp = memory_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
	child->tss.es = 1 * 8;
	child->tss.cs = 2 * 8;
	child->tss.ss = 1 * 8;
	child->tss.ds = 1 * 8;
	child->tss.fs = 1 * 8;
	child->tss.gs = 1 * 8;
	child->tss.eip = (u32_t)tt_main;

	task_run(child, 2, 2);


	boot_sub_procs(__TASK_BAR_CLOCK__);

      while(1){
            /*
		 *マウスのキューはからか?
		 */
            if(IS_FAILURE(queue_size(mouse_queue))){
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
                              //move_layer(Yrws_Master.LAYER_MASTER, window, Yrws_Master.cursor.x-80, Yrws_Master.cursor.y-8);
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
/*
                        if(__MOUSE_DRAGGING__){
                              if(
						Yrws_Master.LAYER_MASTER->layers[Yrws_Master.LAYER_MASTER->layers_map[Yrws_Master.cursor.y * Yrws_Master.screen_width + Yrws_Master.cursor.x]].flags & __WINDOW_LAYER__
						||
						Yrws_Master.flags & __MOUSE_CARRYING_WINDOW__){
                                    move_layer(Yrws_Master.LAYER_MASTER, window->layer, Yrws_Master.cursor.x-80, Yrws_Master.cursor.y-8);
                                    Yrws_Master.flags |= __MOUSE_CARRYING_WINDOW__;
                              }
					}*/
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

      u8_t *wp_buffer, *mc_buffer, *bar_buffer;

	init_yrws_master();
	
      //壁紙のレイヤーを確保
      wall_paper = layer_alloc(Yrws_Master.LAYER_MASTER);
      //マウスカーソルのレイヤーを確保
      mouse_cursor_layer = layer_alloc(Yrws_Master.LAYER_MASTER);
	//タスクバーのレイヤー確保
	task_bar = layer_alloc(Yrws_Master.LAYER_MASTER);

      //壁紙のレイヤーの描画情報を格納するバッファを確保
      wp_buffer = (u8_t *)memory_alloc_4k(memman, binfo->scrnx * binfo->scrny);
      mc_buffer = (u8_t *)memory_alloc(memman, 128);
	bar_buffer = (u8_t *)memory_alloc(memman, binfo->scrnx << 4);

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
	 *タスクバーレイヤーの情報をセット
	 */
	layer_chbuf(task_bar, bar_buffer);
	modify_layer(task_bar, binfo->scrnx, 16, 0xff);
	boxfill8(task_bar->data, task_bar->width, __RGB256COL__(36, 49, 61), 0, 0, task_bar->width, task_bar->height);

      /*
      *レイヤーを移動
      */
      move_layer(Yrws_Master.LAYER_MASTER, wall_paper, 0, 0);
      move_layer(Yrws_Master.LAYER_MASTER, mouse_cursor_layer, Yrws_Master.cursor.x, Yrws_Master.cursor.y);
	move_layer(Yrws_Master.LAYER_MASTER, task_bar, 0, 0);

      /*
      *レイヤーの位置を調整
      */
      layer_ch_position(Yrws_Master.LAYER_MASTER, wall_paper, 0);
      layer_ch_position(Yrws_Master.LAYER_MASTER, mouse_cursor_layer, 2);
	layer_ch_position(Yrws_Master.LAYER_MASTER, task_bar, 3);

      /*
      *レイヤーのタイプ
      */
      wall_paper->flags |= __SYSTEM_LAYER__;
      mouse_cursor_layer->flags |= __SYSTEM_LAYER__;

      redraw_all_layer(Yrws_Master.LAYER_MASTER, wall_paper, 0, 0, wall_paper->width, wall_paper->height);
}

void draw_clock(i8_t hour, i8_t minute){
	
	char time[6];
	zeroclear_8array(time, 6);
	sprintf(time, "%d:%d", hour, minute);
	//boxfill8(task_bar->data, task_bar->width, __RGB256COL__(36, 49, 61), task_bar->width-48, 0, task_bar->width, 16);

	putfonts8_asc(task_bar->data, task_bar->width, task_bar->width-48, 0, __RGB256COL__(36, 49, 61), "      ");
	putfonts8_asc(task_bar->data, task_bar->width, task_bar->width-48, 0, __RGB256COL__(255, 255, 255), time);
	redraw_all_layer(Yrws_Master.LAYER_MASTER, task_bar, 0, 0, task_bar->width, 16);
	
}
