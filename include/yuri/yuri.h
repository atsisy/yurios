#ifndef YURI_H
#define YURI_H

#include "types.h"

/*
 *=======================================================================================
 *IOストリーム
 *=======================================================================================
 */
#define stdin  0xffffffff
#define stdout 0xfffffffe
#define stderr 0xfffffffd

/*
 *=======================================================================================
 *openシステムコールのフラグ
 *=======================================================================================
 */
#define O_RDONLY 0x01
#define O_WRONLY 0x02
#define O_RDWR   0x04
#define O_CREAT  0x08

/*
 *=======================================================================================
 *seekシステムコールのフラグ
 *=======================================================================================
 */
#define SEEK_SET 0x00
#define SEEK_CUR 0x01
#define SEEK_END 0x02

/*
 *=======================================================================================
 *色とか
 *=======================================================================================
 */
#define BLACK		     0
#define RED		       1
#define GREEN		     2
#define YELLOW	     3
#define BLUE		     4
#define PURPLE	     5
#define LIGHT_BLUE   6
#define WHITE	   	   7
#define LIGHT_GRAY   8
#define DARK_RED	   9
#define DARK_GREEN   10
#define DARK_YELLOW	 11
#define DARK_BLUE		 12
#define DARK_PURPLE	 13
#define GRAY_BLUE		 14
#define DARK_GRAY		 15

/*
 *=======================================================================================
 *システムコールのプロトタイプ宣言
 *=======================================================================================
 */
size_t write(int fd, char *buf, int count);
size_t read(int fd, char *buf, int count);
int open(char *pathname, u32_t flags);
void malloc_init();
void *malloc(size_t size);
void free(void *ptr);
int get_key(int mode);
void put_char(int ch);
void exit(i32_t status);
void draw_rect(int start_x, int start_y, int end_x, int end_y, char color);
void sleep(unsigned int timeout);
void newline(void);
u32_t stat(int fd, u32_t *box);
u32_t seek(int fd, off_t offset, int whence);
i32_t close(i32_t fd);
i32_t getca(int *argc, char **argv);

#endif
