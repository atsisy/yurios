#ifndef TYPES_H
#define TYPES_H

#include "value.h"
#include "limits.h"

/*
  ====================================================================
  型定義
  ====================================================================
*/
typedef unsigned int size_t;

typedef unsigned char  u8_t;
typedef unsigned short u16_t;
typedef unsigned int   u32_t;

typedef char  i8_t;
typedef short i16_t;
typedef int   i32_t;

#define TRUE  1
#define true  1
#define FALSE 0
#define false 0


/*
  ====================================================================
  構造体定義
  ====================================================================
*/

/*アセンブリ側*/

struct BOOTINFO { /* 0x0ff0-0x0fff */
	char cyls; /* ブートセクタはどこまでディスクを読んだのか */
	char leds; /* ブート時のキーボードのLEDの状態 */
	char vmode; /* ビデオモード  何ビットカラーか */
	char reserve;
	short scrnx, scrny; /* 画面解像度 */
	u8_t *vram;
};

struct FileInfo{
	unsigned char name[8], ext[3], type;    //ファイル名、拡張子、ファイルのタイプ
	char reserve[10];   //予約する場所らしい
	unsigned int short time, date, clustno; //ファイル情報
	unsigned int size;  //ファイルのサイズ
};

// dsctbl.c
struct SEGMENT_DESCRIPTOR{
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

struct GATE_DESCRIPTOR{
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};

//fifo.c
struct QUEUE {
	/*
	 *実際にデータが蓄えられる変数
	 */
	int *buffer;
	/*
	 *書き込みを行うインデックス
	 */
	int write_index;
	/*
	 *読み込みを行うインデックス
	 */
	int read_index;
	/*
	 *サイズ
	 */
	int size, free, flags;
	struct Process *task;
};

//memory.c
#define MEMMAN_FREES 4090	/*32KB */
#define MEMMAN_ADDR  0x00

//空き情報
struct FREEINFO{
	unsigned int addr, size;
};

//メモリマネージャ構造体
struct MEMMAN{
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};

//timer.c

struct TIMER{
	struct TIMER *next;          //自分の次にタイムアウトするタイマへのポインタ
	unsigned int timeout, status;
	struct QUEUE *fifo;
	unsigned char data;
};

struct TIMER_MASTER {
	unsigned int count;                  //起動からの時間
	unsigned int proximate_timeout;      //次にタイムアウトするタイマのタイムアウト時間
	struct TIMER *proximate_timer;       //次にタイムアウトするタイマへのポインタ
	struct TIMER ALL_TIMERS[MAX_TIMER];  //すべてのタイマを保持する配列
};

//mtask.c

struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int idtr, iomap;
};

/*
 *プロセスを表す構造体
 */
struct Process {
	int sel;        //selはGDTの番号のこと
	int status;     //プロセスの状態
	int level, priority;
	struct QUEUE irq;
	struct TSS32 tss;
	u32_t pid;
	struct Process *parent;
	char proc_name[64];
};

struct PROCESS_LEVEL_FLOOR{
	int THIS_FLOOR_PROCESSES;	       //動作しているタスクの数
	int NOW_RUNNING_PROCESS;		 //現在動作しているタスクがどれだかわかるようにするための変数
	struct Process *ROOMS[MAX_TASKS_LV]; //このフロアの部屋の配列
};

struct PROCESS_MASTER {

	int now_process_level;	//現在動作中のレベル
	char level_change_flag;	//次回のタスクスイッチの時に、レベルも変えたほうがいいかどうか
	/*
	 *プロセスのマンション
	 */
	struct PROCESS_LEVEL_FLOOR PROCESS_APARTMENT[MAX_TASKLEVELS];
	/*
	 *プロセスを直に保持する配列
	 */
	struct Process ALL_PROCESSES[MAX_TASKS];

	u32_t top_pid;
};


#define __TIMER_INTERRUPT_BUFFER_SIZE__ 8
/*
 *=======================================================================================
 *時間割り込みに特化した割り込み管理構造体
 *=======================================================================================
 */
struct Timer_Interrupt {
	struct QUEUE *queue;  //使用するキュー
	struct TIMER *timer;  //使用するタイマ
	i32_t intr_buf[__TIMER_INTERRUPT_BUFFER_SIZE__];    //バッファ
};

struct YRS_SRC {
	char *source;
	char *values;
	i32_t fd;
};

#endif
