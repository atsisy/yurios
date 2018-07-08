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

typedef unsigned int off_t;


typedef _Bool bool;

#define true  1
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

struct FileInfo {
	unsigned char name[8], ext[3], type;    //ファイル名、拡張子、ファイルのタイプ
	char reserve[10];   //予約する場所らしい
	unsigned int short time, date, clustno; //ファイル情報
	unsigned int size;  //ファイルのサイズ
};

// dsctbl.c
struct SEGMENT_DESCRIPTOR {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};

struct GATE_DESCRIPTOR {
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
	int size;
	/*
	*空き容量
	*/
	int free;

	/*
	*キューの状態を表すフラグ
	*/
	int flags;

	/*
	*起こすべきプロセス
	*/
	struct Process *task;

	/*
	*=============================================================================
	*queue_push関数
	*キューにデータをプッシュする関数
	*引数
	*struct QUEUE *fifo
	*=>データをプッシュするキュー
	*int data
	*=>キューにプッシュするデータ
	*=============================================================================
	*/
	int(*push)(struct QUEUE *queue, int data);

	/*
	*=======================================================================================
	*queue_pop関数
	*引数
	*struct QUEUE *queue
	*=>要素を取り出したいキュー
	*=======================================================================================
	*/
	int(*pop)(struct QUEUE *queue);

	/*
	*=======================================================================================
	*queue_size関数
	*キューの現在の要素数を返す関数
	*引数
	*struct QUEUE *queue
	*=>サイズを調べたいキューへのポインタ
	*返り値
	*キューの要素数
	*=======================================================================================
	*/
	int(*element_count)(struct QUEUE *queue);

};

//memory.c
#define MEMMAN_FREES 4090	/*32KB */
#define MEMMAN_ADDR  0x00

//空き情報
struct FREEINFO {
	unsigned int addr, size;
};

//メモリマネージャ構造体
struct MEMMAN {
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};

//timer.c

struct TIMER {
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
	i32_t sel;        //selはGDTの番号のこと
	i32_t status;     //プロセスの状態
	i32_t level;
	i32_t priority;
	struct QUEUE *irq;
	struct TSS32 tss;
	u32_t pid;
	struct Process *parent;
	char proc_name[64];
	u32_t cs_val;
	u32_t ds_val;
	i32_t argc;
	char *cmd_line;
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

/*
 *=======================================================================================
 *ストリーム構造体
 *=======================================================================================
 */
struct Stream {
	char *buffer;
	i32_t write_point;
};

/*
 *=======================================================================================
 *ディレクトリ構造体
 *=======================================================================================
 */
struct Directory {
	char *AbsPath;
	u32_t OwnFD;
};

struct list_head;
typedef struct list_head _list_head;

struct list_head {
        _list_head *next;
        _list_head *prev;
};

typedef struct list_head list_head;

#define container_of(pointer, type, member) \
        ({ \
                const typeof(((type *)0)->member) *__m_ptr = (pointer); \
                (type *)((char *)__m_ptr - offsetof(type, member));     \
        }) 

#define list_get_mother(listp, type, member) \
        (container_of(listp, type, member))

#define list_get_next(listp, type, member) \
        (container_of((listp)->next, type, member))

#define list_get_prev(listp, type, member)              \
        (container_of((listp)->prev, type, member))


/*
 * list_init関数
 * 引数で渡されたリストを初期化する。
 * 自分のnextとprevに自分をセットする。
 */
inline void list_init(list_head *listp)
{
        listp->prev = listp;
        listp->next = listp;
}

/*
 * list_insert関数
 * 第二引数と第三引数の間に第一引数のノードを挿入する
 */
inline void list_insert(list_head *new, list_head *prev, list_head *next)
{
        new->prev = prev;
        new->next = next;
        prev->next = new;
        next->prev = new;
}

/*
 * list_isnert_next_to関数
 * 第一引数のノードを第二引数のnextに挿入する関数
 */
inline void list_insert_next_to(list_head *new, list_head *target)
{
        list_insert(new, target, target->next);
}

/*
 * list_insert_back_to関数
 * 第一引数のノードを第二引数のprevに挿入する関数
 */
inline void list_insert_back_to(list_head *new, list_head *target)
{
        list_insert(new, target->prev, target);
}

/*
 * list_remove関数
 * 第一引数に渡したノードを所属するリストから取り除く関数
 */
inline void list_remove(list_head *listp)
{
        list_head *prev, *next;

        prev = listp->prev;
        next = listp->next;

        prev->next = next;
        next->prev = prev;

        // 初期化
        list_init(listp);
}

/*
 * list_replace関数
 * 第一引数のノードを第二引数のノードで置換する関数
 */
inline void list_replace(list_head *old, list_head *new)
{
        new->prev = old->prev;
        new->next = old->next;
        new->next->prev = new;
        new->prev->next = new;
}

/*
 * list_move_next_to関数
 * 第一引数で受け取ったノードを第二引数のノードのnextに移動する関数
 */
inline void list_move_next_to(list_head *listp, list_head *new_place)
{
        list_remove(listp);
        list_insert_next_to(listp, new_place);
}

/*
 * list_move_back_to関数
 * 第一引数で受け取ったノードを第二引数のノードのprevに移動する関数
 */
inline void list_move_back_to(list_head *listp, list_head *new_place)
{
        list_remove(listp);
        list_insert_back_to(listp, new_place);
}

/*
 * list_empty関数
 * 渡されたノードがダミーノードのみで、そのリストが空であるかどうかを判定する関数
 */
inline bool list_empty(list_head *listp)
{
        return listp->next == listp;
}

/*
 * list_is_singular関数
 * 渡されたノードが属するリストには、
 * ダミーノードと有効なノードの２つ
 * （実質的なデータとしては１つ）であることを判定する関数
 */
inline bool list_is_singular(list_head *listp)
{
        return (!list_empty(listp) && (listp->next == listp->prev));
}

/*
 * list_rotate_left関数
 * 渡されたリストを一個分左向きに回転させる関数
 */
inline void list_rotate_left(list_head *listp)
{
        list_head *first;

        if(!list_empty(listp)){
                first = listp->next;
                list_move_back_to(first, listp);
        }
}

/*
 * list_insert_list関数
 * 第二引数と第三引数の間に第一引数のリストを挿入する関数
 */
inline void list_insert_list(list_head *other_list, list_head *prev, list_head *next)
{
        list_head *first;
        list_head *last;
        
        if(!list_empty(other_list)){
                first = other_list->next;
                last = other_list->prev;

                first->prev = prev;
                last->next = next;

                prev->next = first;
                next->prev = last;
        }
}

/*
 * list_cat関数
 * 第一引数のリストの後に、第二引数のリストを連結し、処理後の先頭の先頭を返す関数
 */
inline list_head *list_cat(list_head *first, list_head *second)
{
        if(!list_empty(second)){
                list_insert_list(second, first->prev, first);
                list_init(second);
        }

        return first;
}

#define list_for_each(_pos, list) \
        for(_pos = (list)->next;_pos != (list);_pos = (pos)->next)

#define list_for_each_reverse(_pos, list)                                       \
        for(_pos = (list)->prev;_pos != (list);_pos = (pos)->prev)

#define list_for_each_safe(_pos, _tmp, list)                              \
        for(_pos = (list)->next, _tmp = (_pos)->next; \
            _pos != (list); \
            _pos = _tmp, _tmp = (pos)->next)

#define list_for_each_reverse_safe(_pos, _tmp, list)            \
        for(_pos = (list)->prev, _tmp = (_pos)->prev;   \
            _pos != (list);                             \
            _pos = _tmp, _tmp = (pos)->prev)

#define list_for_each_entry(_pos, list, member) \
        for(_pos = list_get_mother((list)->next, typeof(*_pos), member); \
            (&_pos)->member != (list); \
            _pos = list_get_mother(_pos->member.next, typeof(*_pos), member))

#define list_for_each_entry_reverse(_pos, list, member)                         \
        for(_pos = list_get_mother((list)->prev, typeof(*_pos), member); \
            (&_pos)->member != (list);                                  \
            _pos = list_get_mother(_pos->member.prev, typeof(*_pos), member))

#define list_for_each_entry_from(_pos, list, member) \
        for(; \
            (&_pos)->member != (list);                                  \
            _pos = list_get_mother(_pos->member.next, typeof(*_pos), member))

#define list_for_each_entry_safe(_pos, _tmp, list, member)               \
        for(_pos = list_get_mother((list)->next, typeof(*_pos), member), \
                    _tmp = list_get_mother((_pos)->member.next, typeof(*_pos), member)); \ \
        (&_pos)->member != (list);                                      \
        _pos = _tmp,                                                    \
                _tmp = list_get_mother((_pos)->member.next, typeof(*_pos), member))

#define list_for_each_entry_reverse_safe(_pos, _tmp, list, member)              \
        for(_pos = list_get_mother((list)->prev, typeof(*_pos), member), \
                    _tmp = list_get_mother((_pos)->member.prev, typeof(*_pos), member)); \ \
        (&_pos)->member != (list);                                      \
        _pos = _tmp,                                                    \
                _tmp = list_get_mother((_pos)->member.prev, typeof(*_pos), member))


#endif
