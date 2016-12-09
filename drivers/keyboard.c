/* キーボード関係 */
#include "../include/kernel.h"
#include "../include/value.h"

struct QUEUE *keybuf;
int keydata0;

void inthandler21(int *esp){  //キーボードからの割り込み
    unsigned char data;
    io_out8(PIC0_OCW2, 0x61);   //IRQ1受付完了をPICに送信, 0x61は、0x60+IRQ番号
    data = io_in8(PORT_KEYDAT);
    queue_push(keybuf, data + keydata0);

    return;
}

#define PORT_KEYSTA				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE	0x47

void wait_KBC_sendready(void){
	//キーボードコントローラがデータ送信可能になるのを待つ
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

void init_keyboard(struct QUEUE *fifo, int data0){
	//キーボードコントローラの初期化
	keybuf = fifo;
	keydata0 = data0;
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}

/*
  ch_keybuf関数の説明
  流し込むキーバッファを変更する。
  引数
  QUEUE構造体のポインタを渡す
*/
void ch_keybuf(struct QUEUE *new_buf){

	keybuf = new_buf;
	return;
}

struct QUEUE *now_keybuf(void){
	return keybuf;
}
