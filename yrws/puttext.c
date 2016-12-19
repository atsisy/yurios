#include "../include/kernel.h"
#include "../include/yrws.h"

/*
 *=======================================================================================
 *ウィンドウにテキストを表示する関数
 *=======================================================================================
 */
bool puttext(struct Layer *window, char *text, u16_t x, u16_t y, char color){
	putfonts8_asc(window->data, window->width, x, 16+y, color, text);
}
