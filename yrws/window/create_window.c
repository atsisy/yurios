#include "../../include/kernel.h"
#include "../../include/yrws.h"
#include "../../include/string.h"

/*
 *=======================================================================================
 *create_window関数
 *ウィンドウを生成する関数
 *=======================================================================================
 */
struct YURI_WINDOW *create_window(char *window_name, u16_t display_x, u16_t display_y, u16_t width, u16_t height){
      /*
      *ウィンドウ用のレイヤーを確保
      */
      struct Layer *window = layer_alloc(Yrws_Master.LAYER_MASTER);

      struct YURI_WINDOW *yr_window = (struct YURI_WINDOW *)memory_alloc(memman, sizeof(struct YURI_WINDOW));

      //レイヤーの描画データ格納場所
      u8_t *window_buf = (u8_t *)memory_alloc_4k(memman, width * height);
      
      /*
      *レイヤーの情報をセット
      */
      layer_chbuf(window, window_buf);
      modify_layer(window, width, height, 255);

	modify_layer(window, width, height, 0xff);
      window->display_x = display_x;
      window->display_y = display_y;

      /*
      *ウィンドウの枠等の核を描画
      */
      draw_window_core(window);
      
      /*
      *レイヤーを指定の位置に持っていく
      */
      move_layer(Yrws_Master.LAYER_MASTER, window, window->display_x, window->display_y);
      layer_ch_position(Yrws_Master.LAYER_MASTER, window, 1);

      /*
      *ウィンドウのラベルを描画
      */
	//puttext(window, window_name, 10, 4, __RGB256COL__(255, 255, 255));
      putfonts8_asc(window->data, window->width, 5, 0, __RGB256COL__(255, 255, 255), window_name);

      /*
      *このレイヤーはウィンドウのレイヤーです
      */
      window->flags |= __WINDOW_LAYER__;

      /*
      *描画
      */
      redraw_all_layer(Yrws_Master.LAYER_MASTER, window, 0, 0, binfo->scrny, binfo->scrny);

      yr_window->layer = window;
      strcpy(yr_window->window_name, window_name, 10);

      return yr_window;
      
}

/*
 *=======================================================================================
 *destroy_window関数
 *ウィンドウを破棄する関数
 *=======================================================================================
 */
void destroy_window(struct YURI_WINDOW *window){
      memory_free(memman, (u32_t)window->layer->data, window->layer->width*window->layer->height);
      free_layer(Yrws_Master.LAYER_MASTER, window->layer);
	memory_free(memman, (u32_t)window, sizeof(struct YURI_WINDOW));
}
