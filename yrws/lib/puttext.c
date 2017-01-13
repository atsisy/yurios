#include "../../include/kernel.h"
#include "../../include/yrws.h"
#include "../../include/string.h"

/*
 *=======================================================================================
 *puttext関数
 *ウィンドウにテキストを表示する関数
 *struct Layer *window
 *描画するウィンドウ
 *char *text
 *描画する文字列
 *u16_t x
 *描画するX位置
 *u16_t y
 *描画するY位置
 *char color
 *色
 *=======================================================================================
 */
char puttext(struct Layer *window, char *text, u16_t x, u16_t y, char color){
	/*
	 *文字列描画
	 */
	putfonts8_asc(window->data, window->width, x, 16+y, color, text);
	/*
	 *再描画処理
	 */
	redraw_all_layer(Yrws_Master.LAYER_MASTER, window,
			     x, y, x + (strlen(text) << 3), y + 32);
	return 0;
}
