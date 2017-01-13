#include "../../include/kernel.h"
#include "../../include/yrws.h"

void DrawRect(struct YURI_WINDOW *window, i32_t x, i32_t y, i32_t width, i32_t height, u8_t color){
	boxfill8(window->layer->data, window->layer->width, color, x, y, x+width, y+height);
	redraw_all_layer(Yrws_Master.LAYER_MASTER, window->layer, window->layer->display_x, window->layer->display_y, window->layer->display_x + width + x, window->layer->display_y + height + y);
}
