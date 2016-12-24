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
*レイヤーの仕様状況フラグ
*/
#define __UNUSED_LAYER__ 0
#define __USED_LAYER__	 1

#define __MOUSE_DRAGGING_FLAG__ 0x01
#define __MOUSE_CARRYING_WINDOW__ 0x02

#define __MOUSE_DRAGGING__ (Yrws_Master.flags & __MOUSE_DRAGGING_FLAG__)

#define __SYSTEM_LAYER__ 0x01
#define __WINDOW_LAYER__ 0x02
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
      i16_t x;
      i16_t y;
};

/*
*重ね合わせ処理に用いるレイヤー構造体
*/
struct Layer {
	u8_t *data;		//レイヤーのデータ
	u16_t width;	//レイヤーの幅
	u16_t height;	//レイヤーの高さ
	i16_t display_x;	//レイヤーがディスプレイのどこにいるか
	i16_t display_y;	//レイヤーがディスプレイのどこにいるか
	i16_t position;	//レイヤーの表面からの順番
	u32_t invisible;	//透明色
	u32_t flags;	//フラグ
};

/*
 *=======================================================================================
 *レイヤーを管理する構造体
 *=======================================================================================
 */

struct Layer_Master {
	i32_t top_layer;
	u8_t *layers_map;
	struct Layer layers[__LAYER_LIMIT__];
	struct Layer *layers_pointers[__LAYER_LIMIT__];
};

/*
 *=======================================================================================
 *ゆりウィンドウシステムを管理する構造体
 *=======================================================================================
 */
struct YRWS_MASTER {
	//ディスプレイの幅
	u16_t screen_width;
	//ディスプレイの高さ
	u16_t screen_height;
	//ビデオラムのアドレス
	u8_t *video_ram;
	//いろんなフラグ
	u32_t flags;

	//レイヤー管理構造体
	struct Layer_Master *LAYER_MASTER;
	//マウスカーソル情報構造体
	struct MOUSE_CURSOR cursor;
	//マウス解読のための構造体
	struct MOUSE_INFO mouse_info;
};

/*
 *=======================================================================================
 *ゆりウィンドウシステムのウィンドウ構造体
 *=======================================================================================
 */
struct YURI_WINDOW {
	struct Layer *layer;
	char window_name[256];
	u32_t flags;
};

/*
 *=======================================================================================
 *yuri imageフォーマットで保存された画像を保持する構造体
 *=======================================================================================
 */
struct YURI_IMAGE{
	u32_t width;  //画像の幅
	u32_t height; //画像の高さ
	u8_t *data;   //画像データ
};

/*
*マウスを初期化する関数
*/
void init_mouse(struct QUEUE *ms_queue);
/*
*マウスの割り込み情報を解読する関数
*/
int decode_mdata(u8_t dat);

/*
*レイヤー関係
*/
struct Layer_Master *layer_master_alloc(struct Layer_Master **master);
struct Layer *layer_alloc(struct Layer_Master *master);
void redraw_all_layer(struct Layer_Master *master, struct Layer *layer, u16_t start_x, u16_t start_y, u16_t end_x, u16_t end_y);
void layer_ch_position(struct Layer_Master *layer_master, struct Layer *layer, int new_position);
void move_layer(struct Layer_Master *master, struct Layer *layer, u16_t x, u16_t y);
void free_layer(struct Layer_Master *master, struct Layer *layer);
u8_t *layer_chbuf(struct Layer *layer, u8_t *buffer);
struct Layer *modify_layer(struct Layer *layer, u16_t width, u16_t height, i32_t invisible);
void redraw_layers(struct Layer_Master *master, i16_t start_x, i16_t start_y, i16_t end_x, i16_t end_y, i32_t redraw_position, i32_t above);
void draw_window_core(struct Layer *layer);
void map_layers(struct Layer_Master *master, i16_t start_x, i16_t start_y, i16_t end_x, i16_t end_y, i32_t redraw_position);

/*
 *yuri window systemのAPI的な
 */
bool puttext(struct Layer *window, char *text, u16_t x, u16_t y, char color);

struct YURI_WINDOW *create_window(char *window_name, u16_t display_x, u16_t display_y, u16_t width, u16_t height);


/*
 *yim画像関連
 */
struct YURI_IMAGE *load_yim(char *file_name);
void draw_yim(struct YURI_WINDOW *window, struct YURI_IMAGE *image, i16_t x, i16_t y);
/*
 *ゆりウィンドウシステムをお管理する構造体
 */
extern struct YRWS_MASTER Yrws_Master;


#endif
