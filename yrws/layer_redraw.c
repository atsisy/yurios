#include "../include/kernel.h"
#include "../include/yrws.h"

/*
 *=======================================================================================
 *redraw_all_layer関数
 *すべてのレイヤーを下から再描画していく関数
 *=======================================================================================
 */
void redraw_all_layer(struct Layer_Master *master){

      int h, x, y, display_x, display_y;
	unsigned char *buf, *vram = binfo->vram, c;
	struct Layer *layer;

      for(h = 0;h <= master->top_layer;h++){

            /*
            *レイヤーとそのデータバッファを得る
            */
		layer = master->layers_pointers[h];
		buf = layer->data;
            /*
            *描画処理
            */
		for(y = 0;y < layer->height;y++){

                  /*
                  *レイヤーの位置を考慮したY座標
                  */
			display_y = layer->display_y + y;

                  for(x = 0;x < layer->width;x++){

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
