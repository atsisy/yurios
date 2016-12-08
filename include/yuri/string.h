#ifndef STRING_H
#define STRING_H

int strcmp(const char *s, const char *t);
unsigned long int strlen(const char *str);
void string_getNext(char *str, char *dst);
char *cut_string(char *str, char *dst, int start_point);
char *strcpy(char *s1, const char *s2);
char *strcat(char *s1, const char *s2);

#endif
