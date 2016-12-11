/* GDTやIDTなどの、 descriptor table 関係 */
#include "../include/kernel.h"
#include "../include/sh.h"
#include "../include/value.h"

void init_gdtidt(void){

	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *) ADR_IDT;
	int i;

	//GDT初期化
	for(i = 0;i <= LIMIT_GDT / 8;i++){
		set_segmdesc(gdt + i, 0, 0, 0);
	}
	set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, AR_DATA32_RW);
	set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, AR_CODE32_ER);
	load_gdtr(LIMIT_GDT, ADR_GDT);

	//IDT初期化
	for(i = 0;i <= LIMIT_IDT / 8;i++){
		set_gatedesc(idt + i, 0, 0, 0);
	}
	load_idtr(LIMIT_IDT, ADR_IDT);

	//ゼロ除算例外が発生したときのハンドラを登録
	set_gatedesc(idt + 0x00, (int) asm_inthandler00, 16, AR_INTGATE32);

	//スタック例外が発生したときのハンドラを登録
	set_gatedesc(idt + 0x0c, (int) asm_inthandler0c, 16, AR_INTGATE32);

	//実行時例外が発生したときの処理を登録する
	set_gatedesc(idt + 0x0d, (int) asm_inthandler0d, 16, AR_INTGATE32);

	//タイマ割り込みが発生したときの処理を登録しておく
	set_gatedesc(idt + 0x20, (int) asm_inthandler20, 16, AR_INTGATE32);

	//キーボード割り込みが発生したときの処理を登録する
	set_gatedesc(idt + 0x21, (int) asm_inthandler21, 16, AR_INTGATE32);

	//0x21 => キーボード割り込みのIRQ番号	asm_inthandler21 => イベントハンドラ登録 16=2*8で、セグメント番号2であることを表す

	//set_gatedesc(idt + 0x2c, (int) asm_inthandler2c, 16, AR_INTGATE32);

	//0x21 => マウス割り込みのIRQ番号	asm_inthandler2c => イベントハンドラ登録 16=2*8で、セグメント番号2であることを表す
	//セグメント番号2はbootpackすべてをカバーするセグメント

	//APIとしてアプリケーションが勝手に使っていいハンドラということをCPUに伝えるために0x60を足している
	set_gatedesc(idt + 0x68, (int) asm_sys_call, 16, AR_INTGATE32 + 0x60);

	return;
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar){

	if(limit > 0xfffff){
		ar |= 0x8000;
		limit /= 0x1000;
	}
	sd->limit_low = limit & 0xfffff;
	sd->base_low = base & 0xfffff;
	sd->base_mid = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high = (base >> 24) & 0xff;
	return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar){

	gd->offset_low = offset & 0xffff;
	gd->selector = selector;
	gd->dw_count = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high = (offset >> 16) & 0xffff;
	return;
}
