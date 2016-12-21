#include "../../include/kernel.h"
#include "../../include/yrws.h"
#include "../../include/sh.h"

extern struct YRWS_MASTER Yrws_Master;
struct QUEUE *mouse_queue;

/*
 *=======================================================================================
 *init_mouse関数
 *マウスを有効化したり、バッファを決定したりする関数
 *=======================================================================================
 */
void init_mouse(struct QUEUE *ms_queue){
      /*
      *マウスを指定する
      */
      mouse_queue = ms_queue;

      /*
      *マウスを有効化する
      */
      wait_KBC_sendready();
      io_out8(PORT_KEYCMD, __KEYCMD_MOUSE__);
      wait_KBC_sendready();
      io_out8(PORT_KEYDAT, 0xf4);

      /*
      *マウスの情報を保持する構造体の初期化
      */
      Yrws_Master.mouse_info.phase = Yrws_Master.mouse_info.x = Yrws_Master.mouse_info.y = 0;

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

	if(Yrws_Master.mouse_info.phase == 0){
		/*
            *初めて割り込みが入る
            */
		if(dat == 0xfa)
			Yrws_Master.mouse_info.phase = 1;

            return false;
	}
	if(Yrws_Master.mouse_info.phase == 1){
		/*
            *1バイト目
            */
		if((dat & 0xc8) == 0x08){
			/* 正しい1バイト目だった */
			Yrws_Master.mouse_info.data[0] = dat;
			Yrws_Master.mouse_info.phase = 2;
		}

		return false;
	}
	if(Yrws_Master.mouse_info.phase == 2){
		/*
            *2バイト目
            */
		Yrws_Master.mouse_info.data[1] = dat;
		Yrws_Master.mouse_info.phase = 3;
		return false;
	}
	if(Yrws_Master.mouse_info.phase == 3){
		/*
            *3バイト目
            */
		Yrws_Master.mouse_info.data[2] = dat;
		Yrws_Master.mouse_info.phase = 1;
		Yrws_Master.mouse_info.button = Yrws_Master.mouse_info.data[0] & 0x07;
		Yrws_Master.mouse_info.x = Yrws_Master.mouse_info.data[1];
		Yrws_Master.mouse_info.y = Yrws_Master.mouse_info.data[2];

		if((Yrws_Master.mouse_info.data[0] & 0x10) != 0)
			Yrws_Master.mouse_info.x |= 0xffffff00;

		if((Yrws_Master.mouse_info.data[0] & 0x20) != 0)
			Yrws_Master.mouse_info.y |= 0xffffff00;

            /*
            *マウスではy方向の符号が画面と反対であることに注意
            */
		Yrws_Master.mouse_info.y = - Yrws_Master.mouse_info.y;

            //マウスのデバッグ用
            /********************************************************************************************
            char str[12] = { 0 };
            boxfill8(binfo->vram, binfo->scrnx, 0, 400, 400, 500, 416);
            sprintf(str, "%x %d %d", Yrws_Master.mouse_info.data[0], Yrws_Master.mouse_info.data[1], Yrws_Master.mouse_info.data[2]);
            putfonts8_asc(binfo->vram, binfo->scrnx, 400, 400, __RGB256COL__(255, 255, 255), str);
            *********************************************************************************************/

            /*
            *右クリック押し込みフラグ(ドラッグに用いる)
            */
            if(
                  Yrws_Master.mouse_info.data[0] == 9
                  &&
                  Yrws_Master.mouse_info.data[1] == 0
                  &&
                  Yrws_Master.mouse_info.data[2] == 0 
            ){
                  /*
                  *ドラッグ開始
                  */
                  Yrws_Master.flags |= __MOUSE_DRAGGING_FLAG__;
            }
            else
            if(
                  Yrws_Master.mouse_info.data[0] == 8
                  &&
                  Yrws_Master.mouse_info.data[1] == 0
                  &&
                  Yrws_Master.mouse_info.data[2] == 0 
            ){
                  /*
                  *ドラッグ終了
                  */
                  Yrws_Master.flags &= ~__MOUSE_DRAGGING_FLAG__;
                  Yrws_Master.flags &= ~__MOUSE_CARRYING_WINDOW__;
            }

		return true;
	}

      /*
      *原因不明で失敗
      */
	return -1;
}
