#include "../include/kernel.h"
#include "../include/yrws.h"

extern struct MOUSE_CURSOR cursor;

static char MOUSE_CURSOR_IMAGE[16] = {
      0x80, 0xc0, 0xe0, 0xf8, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1e, 0x0e, 0x0e, 0x06, 0x03
};
/*
 *=======================================================================================
 *draw_cursor関数
 *マウスカーソルを描画する関数
 *=======================================================================================
 */
void draw_cursor(void){
      u32_t i;
      char point, *vram;

      for(i = 0;i < 16;i++){
		vram = (char *)(binfo->vram) + ((cursor.y + i) * binfo->scrnx) + cursor.x;
            point = MOUSE_CURSOR_IMAGE[i];

		if ((point & 0x80) != 0)
			vram[0] = BLACK;
		if ((point & 0x40) != 0)
			vram[1] = BLACK;
		if ((point & 0x20) != 0)
			vram[2] = BLACK;
		if ((point & 0x10) != 0)
			vram[3] = BLACK;
		if ((point & 0x08) != 0)
			vram[4] = BLACK;
		if ((point & 0x04) != 0)
			vram[5] = BLACK;
		if ((point & 0x02) != 0)
			vram[6] = BLACK;
		if ((point & 0x01) != 0)
			vram[7] = BLACK;
	}
}

void erase_cursor(void){
      boxfill8(binfo->vram, binfo->scrnx, __DEFAULT_WALLPAPER_COLOR__, cursor.x, cursor.y, cursor.x+8, cursor.y+16);
}
