#include "../../include/kernel.h"
#include "../../include/yrws.h"

/*
 *=======================================================================================
 *draw_window関数
 *ウィンドウを描画する関数
 *=======================================================================================
 */
void draw_window_core(struct Layer *layer){
      
      boxfill8(layer->data, layer->width, __RGB256COL__(36, 49, 61), 0, 0, layer->width-48, 16);
      boxfill8(layer->data, layer->width, __RGB256COL__(196, 199, 255), layer->width-48, 0, layer->width-32, 16);
      boxfill8(layer->data, layer->width, __RGB256COL__(232, 255, 148), layer->width-32, 0, layer->width-16, 16);
      boxfill8(layer->data, layer->width, __RGB256COL__(255, 226, 220), layer->width-16, 0, layer->width-1, 16);
      boxfill8(layer->data, layer->width, __RGB256COL__(255, 255, 255), 0, 16, layer->width, layer->height);

}