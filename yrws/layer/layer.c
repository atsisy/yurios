#include "../../include/kernel.h"
#include "../../include/yrws.h"

/*
 *=======================================================================================
 *layer_master_alloc関数
 *layer_masterを確保する関数
 *=======================================================================================
 */
struct Layer_Master *layer_master_alloc(struct Layer_Master **master){

      u32_t i;
      *master = (struct Layer_Master *)memory_alloc_4k(memman, sizeof(struct Layer_Master));

      (*master)->top_layer = -1;

      /*
      *レイヤー情報をマッピングするバッファ
      */
      (*master)->layers_map = (u8_t *)memory_alloc_4k(memman, binfo->scrnx * binfo->scrny);

      /*
      *全てに未使用フラグを立てる
      */
      for(i = 0;i < __LAYER_LIMIT__;i++)
            (*master)->layers[i].flags = __UNUSED_LAYER__;  //未使用フラグ

      return *master;
}

/*
 *=======================================================================================
 *layer_alloc関数
 *新規にレイヤーを確保する関数
 *=======================================================================================
 */
struct Layer *layer_alloc(struct Layer_Master *master){
      struct Layer *layer;
      i32_t i;

      /*
      *確保できるレイヤーを探す
      */
      for(i = 0;i < __LAYER_LIMIT__;i++){
            if(!master->layers[i].flags){
                  layer = &master->layers[i];
                  layer->flags = __USED_LAYER__;
                  layer->position = -1;
                  return layer;
            }
      }

      return NULL;
}

/*
 *=======================================================================================
 *layer_ch_position関数
 *レイヤーの位置を変更する関数
 *=======================================================================================
 */
void layer_ch_position(struct Layer_Master *layer_master, struct Layer *layer, int new_position){

	int h, old = layer->position;

	/*
      *指定が低すぎや高すぎだったら、修正する
      */
	if(new_position > layer_master->top_layer + 1)
		new_position = layer_master->top_layer + 1;

	if(new_position < -1)
		new_position = -1;

      /*
      *高さを設定
      */
      layer->position = new_position;

	/*
      *以下は主にlayers_pointersの並べ替え
      */
	if(old > new_position){
            /*
            *もとより低くなる
            */
		if(new_position >= 0){
			/*
                  *間のものを引き上げる
                  */
			for (h = old; h > new_position; h--) {
				layer_master->layers_pointers[h] = layer_master->layers_pointers[h - 1];
				layer_master->layers_pointers[h]->position = h;
			}

			layer_master->layers_pointers[new_position] = layer;


                  /*
                  *新しい下じきの情報に沿って画面を描き直す
                  */
                  map_layers(layer_master, layer->display_x, layer->display_y,
                        layer->display_x+layer->width, layer->display_y+layer->height, new_position + 1);

		      redraw_layers(layer_master, layer->display_x, layer->display_y,
                        layer->display_x+layer->width, layer->display_y+layer->height, new_position + 1, old);
		}else{
                  /*
                  *非表示化
                  */
			if(layer_master->top_layer > old){
				/*
                        *上になっているものをおろす
                        */
				for(h = old;h < layer_master->top_layer;h++){
					layer_master->layers_pointers[h] = layer_master->layers_pointers[h + 1];
					layer_master->layers_pointers[h]->position = h;
				}

			}

                  /*
                  *表示中の下じきが一つ減るので、一番上の高さが減る
                  */
			layer_master->top_layer--;

                  /*
                  *新しい下じきの情報に沿って画面を描き直す
                  */
                  map_layers(layer_master, layer->display_x, layer->display_y,
                        layer->display_x+layer->width, layer->display_y+layer->height, 0);

                  redraw_layers(layer_master, layer->display_x, layer->display_y,
                        layer->display_x+layer->width, layer->display_y+layer->height, 0, old - 1);
		}
	}else if(old < new_position){
            /*
            *以前よりも高くなる
            */
		if(old >= 0){
			/*
                  *間のものを押し下げる
                  */
			for(h = old;h < new_position;h++){
				layer_master->layers_pointers[h] = layer_master->layers_pointers[h + 1];
				layer_master->layers_pointers[h]->position = h;
			}

			layer_master->layers_pointers[new_position] = layer;

            }else{

                  /*
                  *非表示状態から表示状態へ
                  */

			//上になるものを持ち上げる
			for(h = layer_master->top_layer;h >= new_position;h--){
				layer_master->layers_pointers[h + 1] = layer_master->layers_pointers[h];
				layer_master->layers_pointers[h + 1]->position = h + 1;
			}

                  /*
                  *表示中の下じきが一つ増えるので、一番上の高さが増える
                  */
			layer_master->layers_pointers[new_position] = layer;
			layer_master->top_layer++;
		}

             /*
             *新しい下じきの情報に沿って画面を描き直す
             */
             map_layers(Yrws_Master.LAYER_MASTER, layer->display_x, layer->display_y,
                  layer->display_x+layer->width, layer->display_y+layer->height, new_position);

             redraw_layers(Yrws_Master.LAYER_MASTER, layer->display_x, layer->display_y,
                  layer->display_x+layer->width, layer->display_y+layer->height, new_position, new_position);
	}
	return;
}

/*
 *=======================================================================================
 *move_layer関数
 *レイヤー移動させる関数
 *=======================================================================================
 */
void move_layer(struct Layer_Master *master, struct Layer *layer, u16_t x, u16_t y){

      i16_t old_start_x, old_start_y;

      old_start_x = layer->display_x;
      old_start_y = layer->display_y;

      layer->display_x = x;
      layer->display_y = y;

      /*
      *表示中のレイヤーならば全て書き直す
      */
      if(layer->position >= 0){
            map_layers(master, old_start_x, old_start_y, old_start_x+layer->width, old_start_y+layer->height, 0);
            map_layers(master, x, y, x+layer->width, y+layer->height, layer->position);
            redraw_layers(master, old_start_x, old_start_y, old_start_x+layer->width, old_start_y+layer->height, 0, layer->position - 1);
            redraw_layers(master, x, y, x+layer->width, y+layer->height, layer->position, layer->position);
      }

      return;
}

/*
 *=======================================================================================
 *free_layer関数
 *レイヤーを開放する関数
 *=======================================================================================
 */
void free_layer(struct Layer_Master *master, struct Layer *layer){

      /*
      *レイヤーが表示中なら先に非表示にする
      */
      if(layer->height >= 0)
            layer_ch_position(master, layer, -1);

      layer->flags = __UNUSED_LAYER__;

      return;
 }

/*
 *=======================================================================================
 *layer_chbuf関数
 *レイヤー描画バッファを変更する関数
 *=======================================================================================
 */
u8_t *layer_chbuf(struct Layer *layer, u8_t *buffer){
      layer->data = buffer;
      return buffer;
}

/*
 *=======================================================================================
 *modify_layer関数
 *レイヤーの基本情報をセットする関数
 *=======================================================================================
 */
struct Layer *modify_layer(struct Layer *layer, u16_t width, u16_t height, i32_t invisible){
      layer->width = width;
      layer->height = height;
      layer->invisible = invisible;
      return layer;
}
