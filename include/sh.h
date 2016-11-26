#ifndef YULLIB_H
#define YULLIB_H

#include "bootpack.h"

#define __stdin__  0
#define __stdout__ 1
#define __stderr__ 2


void print(char *str);
void puts(char *str);
void print_value(int value, int x, int y);
void put_char(char ch);

void type_prompt(char *dst, int buffer_limit);

void increase_indent(void);
void increase_length(void);
void erase_a_alphabet(void);

void command_echo(char *inputed_command);
void command_memory(void);
void command_history(void);
void command_ls(void);
void command_cat(char *inputed_command);
void ylsh_clear(void);


void readfat(int *fat, unsigned char *img);
void loadfile(int clustno, int size, char *buf, int *fat, char *img);

extern int MAX_SCROLL;

enum SHELL_MODE{SINGLE, DUAL, QUAD};
extern enum SHELL_MODE shell_mode;

void int2char(char *str, int value);
void int2hex(char *str, int value);
void int2dec(char *str, int value);
void int2str(char *str, int value);

int sprintf(char *str, const char *fmt, ...);
void zero_str(char *str);
int dec_digit(int value, int n);
/*
	シェルモードについて
	0:ノーマルモード(1つ)
	1:デュアルモード(２つ)
	2:クアッドモード(4つ)
*/

/*
	システムコール関数系
*/
size_t do_write(int fd, char *buf, int count);
size_t do_read(int fd, char *buf, int count);

int *system_call(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);

//一時的にここにおいておく。
extern struct QUEUE fifo;
extern char *keys0;
extern char *keys1;

#endif
