#include "../../include/kernel.h"

void enable_mouses(void){
      /*
      *マウスを有効化する
      */
      wait_KBC_sendready();
      io_out8(PORT_KEYCMD, __KEYCMD_MOUSE__);
      wait_KBC_sendready();
      io_out8(PORT_KEYDAT, __ENABLE_MOUSE__);
      return;
}
