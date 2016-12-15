#include "../../include/kernel.h"
#include "../../include/yrws.h"

struct MOUSE_INFO mouse_info;
struct QUEUE *mouse_queue;
int mouse_buf[512];

void init_mouse(void){
      /*
      *マウスのバッファを初期化する
      */
      queue_init(mouse_queue, 512, mouse_buf, NULL);

      /*
      *マウスを有効化する
      */
      wait_KBC_sendready();
      io_out8(PORT_KEYCMD, __KEYCMD_MOUSE__);
      wait_KBC_sendready();
      io_out8(PORT_KEYDAT, __ENABLE_MOUSE__);

      return;
}

/*
 *=======================================================================================
 *mouse_handler関数
 *マウスからの割り込みを受け付ける関数
 *=======================================================================================
 */
void mouse_handler(int *esp){

	u8_t data;
      /*
      *スレーブに受付完了を通知
      */
      io_out8(PIC1_OCW2, 0x64);
      /*
      *マスタに受付完了を通知
      */
      io_out8(PIC0_OCW2, 0x62);

      data = io_in8(PORT_KEYDAT);
      queue_push(mouse_queue, data);

      return;
}

int decode_mdata(u8_t dat){

	if(mouse_info.phase == 0){
		/*
            *初めて割り込みが入る
            */
		if(dat == 0xfa)
			mouse_info.phase = 1;

            return false;
	}
	if(mouse_info.phase == 1){
		/*
            *1バイト目
            */
		if((dat & 0xc8) == 0x08){
			/* 正しい1バイト目だった */
			mouse_info.data[0] = dat;
			mouse_info.phase = 2;
		}

		return false;
	}
	if(mouse_info.phase == 2){
		/*
            *2バイト目
            */
		mouse_info.data[1] = dat;
		mouse_info.phase = 3;
		return false;
	}
	if(mouse_info.phase == 3){
		/*
            *3バイト目
            */
		mouse_info.data[2] = dat;
		mouse_info.phase = 1;
		mouse_info.button = mouse_info.data[0] & 0x07;
		mouse_info.x = mouse_info.data[1];
		mouse_info.y = mouse_info.data[2];

		if((mouse_info.data[0] & 0x10) != 0)
			mouse_info.x |= 0xffffff00;

		if((mouse_info.data[0] & 0x20) != 0)
			mouse_info.y |= 0xffffff00;

            /*
            *マウスではy方向の符号が画面と反対であることに注意
            */
		mouse_info.y = - mouse_info.y;

		return true;
	}

      /*
      *原因不明で失敗
      */
	return -1;
}
