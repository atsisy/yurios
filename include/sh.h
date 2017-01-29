#ifndef YULLIB_H
#define YULLIB_H

#include "kernel.h"

#define __stdin__  0xffffffff
#define __stdout__ 0xfffffffe
#define __stderr__  0xfffffffd


void print(char *str);
i32_t puts(char *str);
void print_value(int value, int x, int y);
void put_char(char ch);
void pError(char *message);

void type_prompt(char *dst, int buffer_limit);
void ntype_prompt(char *dst, int buffer_limit);

void indent_shell(void);
void increase_length(void);
void erase_a_alphabet(void);

void command_print(char *inputed_command);
void command_memory(void);
void command_history(void);
void command_ls(void);
void command_cat(char *inputed_command);
void ylsh_clear(void);
void command_lscpu(void);
 void command_show(char *inputed_command);
void command_writeyim(char *file_name);
void command_rm(char *file_name);
void command_touch(char *file_name);
void command_pwd(void);
void command_mkdir(char *pathname);

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

i32_t osAtoi(char *str);

void load_yrs(struct YRS_SRC *yrs_src);
struct YRS_SRC *yrs_src_init(char *file_name);
void Redirect(char *command);
void RedirectCreateFile(char *FileName);
void RedirectFileAddSub(char *FileName);
i32_t SearchCombinationCommand(char *command);
void RedirectFAdd(char *command);

int *system_call(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);

//一時的にここにおいておく。
extern struct QUEUE fifo;
extern char *keys0;
extern char *keys1;


#endif
