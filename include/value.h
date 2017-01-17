#ifndef VALUE_H
#define VALUE_H

#define __DFONT_ELEMENTS__ 1986

// asmhead.asm
#define ADR_BOOTINFO	0x00000ff0
#define ADR_DISKIMG     0x00100000

//8bitカラー定義
#define BLACK		      0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

// dsctbl.c
#define ADR_IDT		0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT		0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK	0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_TSS32		0x0089
#define AR_INTGATE32	0x008e

// PIC情報
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

//キーボードの状態
#define KEYCMD_LED		0xed
#define PORT_KEYDAT		0x0060
#define PORT_KEYCMD		0x0064


//memory.c
#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000

//shell.c
#define COMMAND_OPTIONS_MAX	256	//コマンドに追加できるオプションの最大数
#define RUNNING_TASK_SPACE	150	//動作中プロセスを表示するエリアの幅(x)

//timer.c
#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

#define NULL_TIMER            0     //未使用のタイマ
#define ALLOC_TIMER      	1	//確保された状態のタイマ
#define USING_TIMER     	2	//使用中のタイマ

/*
 *NULLの定義
 */
#define NULL ((void *)0)

/*
 *プロセスの状態を表す定数
 */
#define NULL_PROCESS     0
#define SLEEPING_PROCESS 1
#define RUNNING_PROCESS  2

/*
 *TSSをGDTの何番から借りてくるか
 */
#define	TASK_GDT	3

#define SUCCESS 1
#define FAILURE 0

/*
 *QUEUEがオーバーランしたときのフラグ
 */
#define FLAGS_OVERRUN		0x0001

/*
 *時刻取得のためのフラグ
 */
#define __SECOND__ 0x00
#define __MINUTE__ 0x02
#define __HOUR__   0x04
#define __DAY__    0x08
#define __MONTH__  0x10
#define __YEAR__   0x20

/*
*YRWS
*/
#define __KEYCMD_MOUSE__ 0xd4
#define __ENABLE_MOUSE__ 0xf4

/*
 *プロセス間通信
 */
#define __SIG_KILL_ME__ 0x01
#define __SIG_KILL_YOU__ 0x02
#define __SIG_SLEEP__ 0x04
#define __SIG_PING__ 0x08

/*
 *ストリームの定数
 */
#define __INPUT_STREAM_SIZE__  4092
#define __OUTPUT_STREAM_SIZE__ 4092

#endif
