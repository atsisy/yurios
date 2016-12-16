#include "../include/kernel.h"
#include "../include/yrws.h"

extern struct MOUSE_CURSOR cursor;

/*
 *=======================================================================================
 *draw_cursor関数
 *マウスカーソルを描画する関数
 *=======================================================================================
 */
void draw_cursor(void){
      //boxfill8(binfo->vram, binfo->scrnx, __RGB256COL__(255, 255, 255), cursor.x, cursor.y, cursor.x+5, cursor.y+5);
      u32_t i;
      for(i = 0;i < 12;i++){
            binfo->vram[(binfo->scrnx*(cursor.y+i))+cursor.x] = 0;
      }
}

void erase_cursor(void){
      boxfill8(binfo->vram, binfo->scrnx, __DEFAULT_WALLPAPER_COLOR__, cursor.x, cursor.y, cursor.x+5, cursor.y+5);
}
