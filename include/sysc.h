#ifndef __SYSC_H__
#define __SYSC_H__

#include "types.h"

/*
  システムコール関数のプロトタイプ宣言
*/
size_t do_write(int fd, char *buf, int count);
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
 *OS側のシステムコール実装
 */
void do_sleep(unsigned int timeout);

u32_t do_fork(void);


int do_open(char *pathname, u32_t flags);
off_t do_seek(int fd, off_t offset, int whence);
u32_t do_stat(int fd, u32_t *box);
i32_t do_close(i32_t fd);

int do_get_key();

#endif

