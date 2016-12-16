#ifndef __YRWS_H__
#define __YRWS_H__

#include "types.h"

/*
*マウスのデータとかをひとまとめにした構造体
*/
struct MOUSE_INFO {
	u8_t data[3];
      u8_t phase;
	i32_t x;
      i32_t y;
      i32_t button;
};

/*
*マウスカーソルの構造体
*/
struct MOUSE_CURSOR {
      u16_t x;
      u16_t y;
};

/*
*デフォルトの背景色
*/
#define __DEFAULT_WALLPAPER_COLOR__ 54

/*
*マウスを初期化する関数
*/
void init_mouse(struct QUEUE *ms_queue);
int decode_mdata(u8_t dat);

#endif
