#include "../../include/kernel.h"
#include "../../include/ata.h"
#include "../../include/yrws.h"
#include "../../include/yrfs.h"
#include "../../include/sh.h"
#include "../../include/sysc.h"

void uchar4tou32(u8_t *data, u32_t *u32);

/*
 *=======================================================================================
 *load_yim関数
 *yuri image画像を読み取る関数
 *=======================================================================================
 */
struct YURI_IMAGE *load_yim(char *file_name){

	struct YURI_IMAGE *image = (struct YURI_IMAGE *)memory_alloc(memman, sizeof(struct YURI_IMAGE));
	u8_t *data = NULL, *data_p = NULL;
	u8_t *header = (u8_t *)memory_alloc(memman, 512);
	
	i32_t fd = do_open(file_name, __O_RDONLY__);
	u32_t i, size = 0;
	struct i_node inode;

	if(fd == -1){
		return NULL;
	}

	iread(&inode, fd);

	/*
	 *ヘッダ読み込み
	 */
	read_ata_sector(&ATA_DEVICE0, inode.begin_address.sector, header, 1);
	
	/*
	 *シグネチャを確認
	 */
      if(header[0] == 'y' && header[1] == 'i' && header[2] == 'm'){
            //シグネチャを読み飛ばす
		header += 3;
		//幅を読み取る
		uchar4tou32(header, &image->width);
		//幅を読み飛ばす
		header += 4;
		//高さを読み取る
		uchar4tou32(header, &image->height);
		//高さを読み飛ばす
		header += 4;
		//画素数を計算
		size = image->height * image->width;

		//画像データ読み込みバッファ
		data = (u8_t *)memory_alloc_4k(memman, size);
		//画像データのバッファを確保
		image->data = (u8_t *)memory_alloc_4k(memman, size);

		data_p = data;

		for(i = 0;i < byte2sectors(size);i++){
			read_ata_sector(&ATA_DEVICE0, inode.begin_address.sector+i, data, 1);
			data += 512;
		}

		/*
		 *画素データを読み取る
		 */

		//ヘッダは飛ばす
		data_p += 11;

	      for(i = 0;i < size;i++){
			image->data[i] = data_p[i];
		}
		memory_free(memman, (u32_t)data_p, size);
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
