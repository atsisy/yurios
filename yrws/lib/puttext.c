#include "../../include/kernel.h"
#include "../../include/yrws.h"
#include "../../include/string.h"

/*
 *=======================================================================================
 *ウィンドウにテキストを表示する関数
 *=======================================================================================
 */
char puttext(struct Layer *window, char *text, u16_t x, u16_t y, char color){
	putfonts8_asc(window->data, window->width, x, 16+y, color, text);
	redraw_all_layer(Yrws_Master.LAYER_MASTER, window,
			     0, 0, /*window->display_x + x + (strlen(text) << 3)*/500, /*window->display_y + y + 32*/500);
	return 0;
}
