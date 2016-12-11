/* 割り込み関係 */
#include "../include/kernel.h"
#include "../include/sh.h"
#include "../include/value.h"

/*
 *=======================================================================================
 *init_pic関数
 *PICを初期化する関数
 *=======================================================================================
 */
void init_pic(void){
    /*
        PIC0_IMR => マスターPIC
        PIC1_IMR => スレーブPIC
    */

    //IMRレジスタの初期化
    io_out8(PIC0_IMR, 0xff); //0xff=>(1111 1111)2で、すべての割り込みが目隠しされる 1だと目隠し
    io_out8(PIC1_IMR, 0xff); //0xff=>(1111 1111)2で、すべての割り込みが目隠しされる

    //PIC0(マスタPIC)のICWレジスタ初期化
    io_out8(PIC0_ICW1, 0x11);   //エッジトリガモード
    io_out8(PIC0_ICW2, 0x20);   //IRQ0~7は、INT20~27で受ける PIC0はIRQ0~7
    io_out8(PIC0_ICW3, 1 << 2); //PIC1(スレーブPIC)はIRQ2にて接続
    io_out8(PIC0_ICW4, 0x01);   //ノンバッファモード

    //PIC1(マスタPIC)のICWレジスタ初期化
    io_out8(PIC1_ICW1, 0x11);   //エッジトリガモード
    io_out8(PIC1_ICW2, 0x28);   //IRQ8~15はINT28~2fで受ける PIC1はIRQ8~15
    io_out8(PIC1_ICW3, 2);      //PIC1(スレーブPIC)はマスタPICのIRQ2に接続
    io_out8(PIC1_ICW4, 0x01);   //ノンバッファモード

    io_out8(PIC0_IMR, 0xfb);    //11111011 PIC1以外すべて禁止
    io_out8(PIC1_IMR, 0xff);    //11111111 すべての割り込みを受け付けない

    return;
}

/*
 *=======================================================================================
 *inthandler2c関数
 *マウスからの割り込みを受け付ける関数
 *=======================================================================================
 */
void inthandler2c(int *esp){

	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "mouse");
	for (;;) {
		io_hlt();
	}
}

/*
 *=======================================================================================
 *inthandler0c関数
 *プログラムが変なことをしたら飛んでくる割り込みを受け付ける関数
 *一般保護例外
 *=======================================================================================
 */
int *inthandler00(int *esp){

	struct Process *proc = task_now();
	puts("Segmentation fault");  //異常終了することをシェルに出力
	puts("division by zero exception.");
	return &(proc->tss.esp0);      //異常終了実行

}


/*
 *=======================================================================================
 *inthandler0c関数
 *プログラムが変なことをしたら飛んでくる割り込みを受け付ける関数
 *一般保護例外
 *=======================================================================================
 */
int *inthandler0c(int *esp){

  struct Process *proc = task_now();
  puts("Segmentation fault");  //異常終了することをシェルに出力
  puts("general protected exception.");
  return &(proc->tss.esp0);      //異常終了実行

}

/*
 *=======================================================================================
 *inthandler0d関数
 *プログラムが変なことをしたら飛んでくる割り込みを受け付ける関数
 *スタック例外
 *=======================================================================================
 */
int *inthandler0d(int *esp){

  struct Process *proc = task_now();
  puts("Segmentation fault");  //異常終了することをシェルに出力
  puts("stack exception.");
  return &(proc->tss.esp0);      //異常終了実行
}
