#include "../include/kernel.h"
#include "../include/yrws.h"

/*
 *=======================================================================================
 *redraw_layers関数
 *redraw_all_layerで、範囲を指定し高速化した関数
 *=======================================================================================
 */
void map_layers(struct Layer_Master *master, i16_t start_x, i16_t start_y, i16_t end_x, i16_t end_y, i32_t redraw_position){

      i32_t h, x, y, display_x, display_y, start_x_sub, start_y_sub, end_x_sub, end_y_sub, layer_id;
	unsigned char *buf, *vram = binfo->vram, *lmap = master->layers_map, c;
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
            layer_id = layer - master->layers;
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
			      lmap[(display_y * binfo->scrnx) + display_x] = c;

			}
		}
	}

      return;
}
