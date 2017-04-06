#ifndef STRING_H
#define STRING_H

#include "types.h"

int strcmp(char *s, char *t);
int strlen(const char *str);
char *string_getNext(char *str, char *dst);
void cut_string(char *str, char *dst, int start_point);
char *strcpy(char *dst, const char *str);
char *strcat(char *s1, const char *s2);
void *memcpy(void *s1, const void *s2, size_t size);
i32_t SearchStringFirst(char *str, char alphabet);
i32_t SearchStringTail(char *str, char alphabet);
char GetStringTail(char *str);
void gline(int fd, char *line);
struct StringBuffer *create_string_buffer(const char *init_str);
u8_t StringBuffer_Append(struct StringBuffer *str_buf, const char *str);


struct StringBuffer {
	char *buffer;
	u16_t write_point;
};


/*
 *=======================================================================================
 *マクロとか
 *=======================================================================================
 */
#define __SHRINK_STR_MACRO__(str) while(*str == ' '){ str++; }

#endif
