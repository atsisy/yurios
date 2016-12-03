#ifndef YURI_H
#define YURI_H

#include "../types.h"

#define stdin  0xffffffff
#define stdout 0xfffffffe
#define stderr 0xfffffffd

#define O_RDONLY 0x01
#define O_WRONLY 0x02
#define O_RDWR   0x04
#define O_CREAT  0x08

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

size_t write(int fd, char *buf, int count);
size_t read(int fd, char *buf, int count);
int open(char *pathname, u32_t flags);
void malloc_init();
void *malloc(size_t size);
void free(void *ptr);
int get_key(int mode);
void put_char(int ch);
void exit(void);
void draw_rect(int start_x, int start_y, int end_x, int end_y, char color);
void sleep(unsigned int timeout);
void newline(void);

#endif
