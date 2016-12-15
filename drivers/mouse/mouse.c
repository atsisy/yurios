#include "../../include/kernel.h"

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
 *inthandler2c関数
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
