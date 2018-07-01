#ifndef __SH_H__
#define __SH_H__

#include "kernel.h"

#define __stdin__  0xffffffff
#define __stdout__ 0xfffffffe
#define __stderr__  0xfffffffd

void readfat(int *fat, unsigned char *img);
void loadfile(int clustno, int size, char *buf, int *fat, char *img);

extern int MAX_SCROLL;

enum SHELL_MODE{SINGLE, DUAL, QUAD};
extern enum SHELL_MODE shell_mode;

#define SCREEN_WIDTH 80

int sprintf(char *str, const char *fmt, ...);
void zero_str(char *str);
i32_t osAtoi(char *str);

int *system_call(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);

//一時的にここにおいておく。
extern struct QUEUE fifo;

#endif
