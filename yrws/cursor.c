#include "../include/kernel.h"
#include "../include/yrws.h"

#define __INVISIBLE__ 0xff

/*
 *=======================================================================================
 *draw_cursor関数
 *マウスカーソルを描画する関数
 *=======================================================================================
 */
void init_mscursor(struct Layer *layer){
      u32_t i;
      char point, *vram = layer->data;
      static char MOUSE_CURSOR_IMAGE[16] = {
            0x80, 0xc0, 0xe0, 0xf8, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1e, 0x0e, 0x0e, 0x06, 0x03
      };

      for(i = 0;i < 16;i++){
            point = MOUSE_CURSOR_IMAGE[i];

		if((point & 0x80) != 0)
			vram[0] = BLACK;
            else
                  vram[0] = __INVISIBLE__;
		if((point & 0x40) != 0)
			vram[1] = BLACK;
            else
                  vram[1] = __INVISIBLE__;
		if((point & 0x20) != 0)
			vram[2] = BLACK;
            else
                  vram[2] = __INVISIBLE__;
            if((point & 0x10) != 0)
			vram[3] = BLACK;
            else
                  vram[3] = __INVISIBLE__;
		if((point & 0x08) != 0)
			vram[4] = BLACK;
            else
                  vram[4] = __INVISIBLE__;
		if((point & 0x04) != 0)
			vram[5] = BLACK;
            else
                  vram[5] = __INVISIBLE__;
            if((point & 0x02) != 0)
			vram[6] = BLACK;
            else
                  vram[6] = __INVISIBLE__;
            if((point & 0x01) != 0)
			vram[7] = BLACK;
            else
                  vram[7] = __INVISIBLE__;

            //前にすすめる
            vram += 8;
	}

}
