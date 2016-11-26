#ifndef YURI_H
#define YURI_H

#include "types.h"

#define stdin  0
#define stdout 1
#define stderr 2

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
void malloc_init();
void *malloc(size_t size);
void free(void *ptr);
int get_key(int mode);
void put_char(int ch);
void exit(void);
void draw_rect(int start_x, int start_y, int end_x, int end_y, char color);
void sleep(unsigned int timeout);

#endif
