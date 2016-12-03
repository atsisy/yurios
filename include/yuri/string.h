#ifndef STRING_H
#define STRING_H

int strcmp(char *s, char *t);
int strlen(char *str);
void string_getNext(char *str, char *dst);
void cut_string(char *str, char *dst, int start_point);
void strcpy(char *copied_str, char *str, int length);
char *strcat(char *s1, const char *s2);
void getline(int fd, char *line);

#endif
