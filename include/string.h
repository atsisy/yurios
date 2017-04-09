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
i8_t ystring_insert(char **str_pointer, const char *insert_str, u32_t index);
i8_t y_shrink_loop_char(char **str_pointer, char target_char);
i8_t ystring_replace_char(char *str, char target, char alternative);

struct StringBuffer *create_string_buffer(const char *init_str);
i8_t StringBuffer_Append(struct StringBuffer *str_buf, const char *str);
char StringBuffer_CharAt(struct StringBuffer *str_buf, u32_t index);
size_t StringBuffer_Length(struct StringBuffer *str_buf);
i8_t StringBuffer_Clean(struct StringBuffer *str_buf);
i8_t StringBuffer_Free(struct StringBuffer *str_buf);
i8_t StringBuffer_Insert(struct StringBuffer *str_buf, const char *str, int index);
i8_t StringBuffer_ReplaceChar(struct StringBuffer *str_buf, char target, char alternative);
i8_t StringBuffer_ShrinkLoopChar(struct StringBuffer *str_buf, char target_char);

struct StringBuffer {
	char *buffer;
};


/*
 *=======================================================================================
 *マクロとか
 *=======================================================================================
 */
#define __SHRINK_STR_MACRO__(str) while(*str == ' '){ str++; }

#endif
