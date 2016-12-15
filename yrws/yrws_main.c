#include "../include/kernel.h"
#include "../include/yrws.h"
#include "../include/sh.h"

extern struct MOUSE_INFO mouse_info;
extern struct QUEUE *mouse_queue;
struct MOUSE_CURSOR cursor;

void yrsw_main(){

      struct Process *me = task_now();
      u32_t data;


      return;
      while(1){
            /*
            *マウスのキューはからか?
            */
            if(!queue_size(mouse_queue)){
			/*
			 *割り込み来ないから寝る
			 */
			task_sleep(me);
			io_sti();
            }else{
                  /*
                  *何らかの割り込みが来た
                  */
			data = queue_pop(mouse_queue);
			io_sti();

			if(decode_mdata(&mouse_info, data) != 0){
				/*
                        *データが3バイト揃ったので表示
                        */
                        if((mouse_info.button & 0x01) != 0){
					/*
                              *左ボタン
                              */
				}
				if((mouse_info.button & 0x02) != 0){
				      /*
                              *右ボタン
                              */
				}
				if((mouse_info.button & 0x04) != 0){
					/*
                              *中央ボタン
                              */
				}
				/*
                        *マウスカーソルの移動
                        */
				cursor.x += mouse_info.x;
				cursor.y += mouse_info.y;



                        //Xの限界
				if(cursor.x < 0)
					cursor.x = 0;

                        //Yの限界
				if(cursor.y < 0)
					cursor.y = 0;

                        //逆方向のXの限界
                        if(cursor.x > binfo->scrnx - 1)
					cursor.x = binfo->scrnx - 1;

                        //逆方向のYの限界
				if(cursor.y > binfo->scrny - 1)
					cursor.y = binfo->scrny - 1;

                        print_value(6, cursor.x, cursor.y);
			}
            }
      }
}
