#include "../include/kernel.h"
#include "../include/yrws.h"

/*
 *=======================================================================================
 *layer_master_alloc関数
 *layer_masterを確保する関数
 *=======================================================================================
 */
struct Layer_Master *layer_master_alloc(void){

      u32_t i;
      struct Layer_Master *master = (struct Layer_Master *)memory_alloc_4k(memman, sizeof(Layer_Master));

      master->top_layer = -1;

      /*
      *全てに未使用フラグを立てる
      */
      for(i = 0;i < __LAYER_LIMIT__;i++)
            master->layers[i].flags = __UNUSED_LAYER__;  //未使用フラグ

      return master;
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
 *mouse_handler関数
 *マウスからの割り込みを受け付ける関数
 *=======================================================================================
 */
 void modify_layer_info(struct Layer *layer, u8_t *arg_data, u16_t x, u16_t y, u32_t invi_col){
       layer->data = arg_data;
       layer->display_x = x;
       layer->display_y = y;
       layer->invisible = invi_col;
       return;
 }
