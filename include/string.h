#ifndef STRING_H
#define STRING_H

int strcmp(char *s, char *t);
int strlen(const char *str);
void string_getNext(char *str, char *dst);
void cut_string(char *str, char *dst, int start_point);
char *strcpy(char *dst, const char *str);
char *strcat(char *s1, const char *s2);

#endif
