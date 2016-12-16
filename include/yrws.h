#ifndef __YRWS_H__
#define __YRWS_H__

#include "types.h"

/*
*デフォルトの背景色
*/
#define __DEFAULT_WALLPAPER_COLOR__ 54
/*
*レイヤーの最大数
*/
#define __LAYER_LIMIT__ 1024
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
*重ね合わせ処理に用いるレイヤー構造体
*/
struct Layer {
	u8_t *data;		//レイヤーのデータ
	u16_t width;	//レイヤーの幅
	u16_t height;	//レイヤーの高さ
	u16_t display_x;	//レイヤーがディスプレイのどこにいるか
	u16_t display_y;	//レイヤーがディスプレイのどこにいるか
	u16_t position;	//レイヤーの表面からの順番
	u32_t invisible;	//透明色
	u32_t flags;	//フラグ
};

/*
*
*/
struct Layer_Master {
	i32_t top_layer;
	struct Layer layers[__LAYER_LIMIT__];
	struct Layer *layers_pointers[__LAYER_LIMIT__];
};



/*
*マウスを初期化する関数
*/
void init_mouse(struct QUEUE *ms_queue);
int decode_mdata(u8_t dat);

#endif
