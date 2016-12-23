#include "../../include/kernel.h"
#include "../../include/yrws.h"
#include "../../include/yrfs.h"
#include "../../include/sh.h"

/*
 *=======================================================================================
 *load_yim関数
 *yuri image画像を読み取る関数
 *=======================================================================================
 */
struct YURI_IMAGE *load_yim(char *file_name){

	struct YURI_IMAGE *image = (struct YURI_IMAGE *)memory_alloc(memman, sizeof(struct YURI_IMAGE));
	u8_t *data = (u8_t *)memory_alloc(memman, 512);
	i32_t fd = do_open(file_name, __O_RDONLY__);
	u32_t i, size;

	if(fd == -1)
		return NULL;
	do_read(fd, data, 1);

	/*
	 *シグネチャを確認
	 */
      if(data[0] == 'y' && data[1] == 'i' && data[2] == 'm'){
		//シグネチャを読み飛ばす
		data += 3;
		//幅を読み取る
		image->width = *((u32_t *)data);
		//幅を読み飛ばす
		data += 4;
		//高さを読み取る
		image->height = *((u32_t *)data);
		//高さを読み飛ばす
		data += 4;
		//画素数を計算
		size = image->height * image->width;
		//画像データのバッファを確保
		image->data = (u8_t *)memory_alloc(memman, size);

		/*
		 *画素データを読み取る
		 */
	      for(i = 0;i < size;i++){
			image->data[i] = data[i];
		}
	}

	return image;
}

/*
 *=======================================================================================
 *draw_yim関数
 *ウィンドウに画像を描画する関数
 *=======================================================================================
 */
void draw_yim(struct YURI_WINDOW *window, struct YURI_IMAGE *image, i16_t x, i16_t y){
	u16_t cx, cy;

	y += 16;

	/*
	 *描画
	 */
	for(cy = 0;cy < image->height;cy++){
		for(cx = 0;cx < image->width;cx++){
			window->layer->data[(y+cy)*window->layer->width + (cx+x)] = image->data[cy*image->height+cx];
		}
	}

	return;
}
