#include "../../include/kernel.h"
#include "../../include/ata.h"
#include "../../include/yrws.h"
#include "../../include/yrfs.h"
#include "../../include/sh.h"

void uchar4tou32(u8_t *data, u32_t *u32);

/*
 *=======================================================================================
 *load_yim関数
 *yuri image画像を読み取る関数
 *=======================================================================================
 */
struct YURI_IMAGE *load_yim(char *file_name){

	struct YURI_IMAGE *image = (struct YURI_IMAGE *)memory_alloc(memman, sizeof(struct YURI_IMAGE));
	u8_t *data = (u8_t *)memory_alloc_4k(memman, 200000), *data_p;
	i32_t fd = do_open(file_name, __O_RDONLY__);
	u32_t i, size;
	struct i_node inode;

	if(fd == -1){
		return NULL;
	}

	data_p = data;
	iread(&inode, fd);
	
	for(i = 0;i < byte2sectors(156311);i++){
		read_ata_sector(&ATA_DEVICE0, inode.begin_address.sector+i, data_p, 1);
		data_p += 512;
	}

	/*
	 *シグネチャを確認
	 */
      if(data[0] == 'y' && data[1] == 'i' && data[2] == 'm'){
            //シグネチャを読み飛ばす
		data += 3;
		//幅を読み取る
		uchar4tou32(data, &image->width);
		//幅を読み飛ばす
		data += 4;
		//高さを読み取る
		uchar4tou32(data, &image->height);
		//高さを読み飛ばす
		data += 4;
		//画素数を計算
		size = image->height * image->width;
		//画像データのバッファを確保
		image->data = (u8_t *)memory_alloc_4k(memman, size);

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
	u32_t i = 0;

	/*
	 *ウィンドウのラベルバー部分を加算
	 */
	y += 16;

	/*
	 *描画
	 */
	for(cy = 0;cy < image->height;cy++){
		for(cx = 0;cx < image->width;cx++){
			window->layer->data[(y+cy)*window->layer->width + (cx+x)] = image->data[i];
			i++;
		}
	}

	redraw_all_layer(Yrws_Master.LAYER_MASTER, window->layer, 0, 0,
			     window->layer->width, window->layer->height);
	
	return;
}
