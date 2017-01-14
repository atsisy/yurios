#ifndef __SYSC_H__
#define __SYSC_H__

/*
 *=======================================================================================
 *システムコール関数のプロトタイプ宣言
 *=======================================================================================
 */

#include "types.h"

/*
 *writeシステムコール
 */
size_t do_write(int fd, char *buf, int count);

/*
 *readシステムコール
 */
size_t do_read(int fd, char *buf, int count);

/*
 *=======================================================================================
 *do_gettime関数
 *現在時刻をRCTにアクセスして得る関数
 *フラグによって何を返すか決める
 *=======================================================================================
 */
u16_t do_gettime(char flag);

/*
 *sleepシステムコール
 */
void do_sleep(unsigned int timeout);

/*
 *forkシステムコール（未完成）
 */
u32_t do_fork(void);

/*
 *openシステムコール
 */
int do_open(char *pathname, u32_t flags);

/*
 *seekシステムコール
 */
off_t do_seek(int fd, off_t offset, int whence);

/*
 *statシステムコール
 */
u32_t do_stat(int fd, u32_t *box);

/*
 *closeシステムコール
 */
i32_t do_close(i32_t fd);

/*
 *get_keyシステムコール
 */
int do_get_key();

#endif

