#include "../include/kernel.h"
#include "../include/yrws.h"


/*
 *=======================================================================================
 *redraw_all_layer関数
 *すべてのレイヤーを下から再描画していく関数
 *=======================================================================================
 */
void redraw_all_layer(struct Layer_Master *master, struct Layer *layer, u16_t start_x, u16_t start_y, u16_t end_x, u16_t end_y){
      //表示中のレイヤーですか？
      if(layer->position >= 0){
            redraw_layers(master, layer->display_x+start_x, layer->display_y+start_y,
                   layer->display_x+end_x, layer->display_y+end_y, layer->position);
      }
	return;
}

/*
 *=======================================================================================
 *redraw_layers関数
 *redraw_all_layerで、範囲を指定し高速化した関数
 *=======================================================================================
 */
void redraw_layers(struct Layer_Master *master, i16_t start_x, i16_t start_y, i16_t end_x, i16_t end_y, i32_t redraw_position){

      i32_t h, x, y, display_x, display_y, start_x_sub, start_y_sub, end_x_sub, end_y_sub;
	unsigned char *buf, *vram = Yrws_Master.video_ram, c;
	struct Layer *layer;

      if(start_x < 0)
            start_x = 0;
      if(start_y < 0)
            start_y = 0;
      if(end_x > binfo->scrnx)
            end_x = binfo->scrnx;
      if(end_y > binfo->scrny)
            end_y = binfo->scrny;

      for(h = redraw_position;h <= master->top_layer;h++){

            /*
            *レイヤーとそのデータバッファを得る
            */
		layer = master->layers_pointers[h];
		buf = layer->data;

            start_x_sub = start_x - layer->display_x;
            start_y_sub = start_y - layer->display_y;
            end_x_sub = end_x - layer->display_x;
            end_y_sub = end_y - layer->display_y;

            if(start_x_sub < 0)
                  start_x_sub = 0;
            if(start_y_sub < 0)
                  start_y_sub = 0;

            if(end_x_sub > layer->width)
                  end_x_sub = layer->width;
            if(end_y_sub > layer->height)
                  end_y_sub = layer->height;
            /*
            *描画処理
            */
		for(y = start_y_sub;y < end_y_sub;y++){

                  /*
                  *レイヤーの位置を考慮したY座標
                  */
			display_y = layer->display_y + y;

                  for(x = start_x_sub;x < end_x_sub;x++){

                        /*
                        *レイヤーの位置を考慮したX座標
                        */
				display_x = layer->display_x + x;

                        /*
                        *レイヤーの色をとってくる
                        */
                        c = buf[(y * layer->width) + x];
                        if(c != layer->invisible)
			      vram[(display_y * binfo->scrnx) + display_x] = c;

			}
		}
	}

      return;
}
