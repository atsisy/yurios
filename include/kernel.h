#ifndef KERNEL_H
#define KERNEL_H

#include "types.h"

//bootpack.c
extern struct MEMMAN *memman;
extern const struct BOOTINFO *binfo;
extern struct QUEUE fifo;

extern char sent_command[1024];
extern char dfont[__DFONT_ELEMENTS__];	//フォントデータ
/* プロトタイプ宣言 */

/* nasmfunc.asm */
void io_hlt(void);	//hltするための関数
void print_a(void);
void io_cli(void);	//割り込み許可フラグを0にして割り込み禁止にする
void io_sti(void);	//割り込み許可フラグを1にして割り込み許可にする
void io_stihlt(void);
void io_out8(int port, int data);	//デバイスに信号を送る
int io_in8(int port);	//デバイスから信号をもらう
void io_out16(int port, int data);	//デバイスに信号を送る
int io_in16(int port);	//デバイスから信号をもらう
void io_out32(int port, int data);	//デバイスに信号を送る
int io_in32(int port);	//デバイスから信号をもらう
int io_load_eflags(void);	//eflagsレジスタの値を返す関数
void io_store_eflags(int eflags);	//eflagsに値を代入する関数
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void asm_inthandler00(void);
void asm_inthandler0c(void);
void asm_inthandler0d(void);
void asm_inthandler21(void);
void asm_inthandler2c(void);
void asm_inthandler20(void);
unsigned int memtest_sub(unsigned int start, unsigned int end);
int load_cr0(void);
void store_cr0(unsigned long cr0);
int load_cr3(void);
void store_cr3(unsigned long cr3);
void flush_tlb(unsigned long addr);
void farjmp(int eip, int cs);
void farcall(int eip, int cs);
void load_tr(int tr);
void asm_put_char(void);
void asm_sys_call(void);
i32_t cpu_brand_string(char *string);
void run_app(int eip, int cs, int esp, int ds, int *tss_esp0);	//アプリケーション起動
void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);

void put_character(u8_t c, u8_t fcolor, u8_t backcolor, u8_t x, u8_t y);

#define TEXT_VIDEO_RAM_ADDR 0xb8000
#define TEXT_SCREEN_WIDTH 80
#define TEXT_SCREEN_HEIGHT 25
/*
 * フレームバッファのポート
 */
// 情報を送信するためのポート
#define FRAMEBUF_COMMAND_PORT 0x03d4
// フレームバッファに対する設定
#define FRAMEBUF_DATA_PORT 0x03d5

/*
 * フレームバッファに対する信号
 */
#define FRAMEBUF_HIGH_BYTE 14
#define FRAMEBUF_LOW_BYTE 15

// dsctbl.c

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);


//int.c
void init_pic(void);
int *inthandler0d(int *esp);		//例外発生時の割り込みハンドラ
void inthandler2c(int *esp);	//マウス割り込みだけど今は使わない

//fifo.c

int queue_init(struct QUEUE *queue, int size, int *buf, struct Process *process);
int queue_push(struct QUEUE *queue, int data);
int queue_pop(struct QUEUE *queue);
int queue_size(struct QUEUE *queue);


//keyboard.c
void inthandler21(int *esp);
void wait_KBC_sendready(void);
void init_keyboard(struct QUEUE *fifo, int data0);
void ch_keybuf(struct QUEUE *new_buf);
struct QUEUE *now_keybuf(void);

void puts(const char *str);
void yksh_init();
void printk(const char *format, ...);
void put_char(char ch);

inline char kitoa(i8_t num)
{
        return num + 0x30;
}

//timer.c

void init_pit(void);
void inthandler20(int *esp);
void timer_settime(struct TIMER *timer, unsigned int timeout);
struct TIMER *timer_alloc(void);
void timer_free(struct TIMER *timer);
void timer_init(struct TIMER *timer, struct QUEUE *queue, unsigned char data);
extern struct TIMER_MASTER timer_master;



//mtask.c

extern struct TIMER *task_timer;
struct Process *task_init(struct MEMMAN *memmann, char *p_name);
struct Process *task_alloc(char *p_name);
void task_switch(void);
void task_run(struct Process *task, int level, int priority);
void task_sleep(struct Process *task);
struct Process *task_now(void);
void task_remove(struct Process *task);
void task_switchsub(void);
u32_t issue_pid();
void task_add(struct Process *task);


struct yksh_controler {
        u16_t cp_x;
        u16_t cp_y;
};


//history.c
i32_t history_init(void);
void add_history(char *inputed_command);

extern char **history;
extern int put_index;
extern char is_fulled;

extern char memo[256];

//proc_manager.c
void proc_manager_init(void);
void proc_manager_main(void);
void create_process(int (*function)(char *), int level, int priority);

/*
 *t_clock.c
 */

void cpu_vendor(u32_t *ebx, u32_t *ecx, u32_t *edx);
void cpu_cache_sub(u32_t *ebx, u32_t *ecx, u32_t *edx);

/*
 *メモリを読む関数
 */
size_t read_mem2hd(char *file_name, char *buffer, u32_t length);

u32_t fat_getsize(char *file_name);

/*
 *ファイルの最後に書き込む関数
 */
void fadd(int fd, char *buffer);

/*
 *フロッピーのFATフォーマットのファイルのサイズを調べる関数
 */
size_t GetFlpFileSize(char *FileName);

/*
 *256色を表示するときのRGB値を決定する関数マクロ
 */
#define __RGB256COL__(r, g, b) (16+(r/51)+(6*(g/51))+(36*(b/51)))

/*
*ゼロクリアする関数
*/
void zeroclear_8array(i8_t *array, u32_t length);
void zeroclear_16array(i16_t *array, u32_t length);
void zeroclear_32array(i32_t *array, u32_t length);

/*
 *=======================================================================================
 *Timer_Interrupt構造体関係
 *=======================================================================================
 */
struct Timer_Interrupt *alloc_TimerIntr(struct Process *proc, i32_t identifier);
void free_TimerIntr(struct Timer_Interrupt *timer_intr);
i32_t TimerIntr_SetTime(struct Timer_Interrupt *timer_intr, i32_t time_limit);
char TimerIntr_came(struct Timer_Interrupt *timer_intr);

char **extend(char *line);
void end(void);
void kernel_send(struct Process *proc, u32_t msg);

/*
 *=======================================================================================
 *ストリーム関係
 *=======================================================================================
 */
void InitStreams();
char *GetOutputStream();
char *GetInputStream();
void WriteOutputStream(char *str);
void WriteErrorStream(char *str);
void ResetInputStream();
void ResetOutputStream();
void ResetErrorStream();

#endif
