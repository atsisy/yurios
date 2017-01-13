#include "../../include/kernel.h"
#include "../../include/yrws.h"

/*
 *=======================================================================================
 *DrawRect関数
 *長方形を描画する関数
 *struct YURI_WINDOW *window
 *描画するウィンドウ
 *i32_t x
 *描画するX位置
 *i32_t y
 *描画するY位置
 *i32_t width
 *描画する幅
 *i32_t height
 *描画する高さ
 *u8_t color
 *長方形の色
 *=======================================================================================
 */
void DrawRect(struct YURI_WINDOW *window, i32_t x, i32_t y, i32_t width, i32_t height, u8_t color){
	/*
	 *描画
	 */
	boxfill8(window->layer->data, window->layer->width, color, x, y, x+width, y+height);
	/*
	 *再描画処理
	 */
	redraw_all_layer(Yrws_Master.LAYER_MASTER, window->layer, x, y, window->layer->display_x + width + x, window->layer->display_y + height + y);
}

/*
 *=======================================================================================
 *BackGroundColor関数
 *ウィンドウの背景色をn変更する関数
 *struct YURI_WINDOW *window
 *背景色を変更するウィンドウ
 *u8_t color
 *背景色
 *=======================================================================================
 */
void BackGroundColor(struct YURI_WINDOW *window, u8_t color){
	DrawRect(window, 0, 16, window->layer->width, window->layer->height, color);
}
