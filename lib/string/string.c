#include "../../include/types.h"

int strcmp(const char *s, const char *t){
	int i;
	for(i = 0;s[i] != '\0';i++){
		if(s[i] != t[i]){
			return 0;
		}
	}
	for(i = 0;t[i] != '\0';i++){
		if(s[i] != t[i]){
			return 0;
		}
	}
	return 1;
}

int strlen(char *str){
	u32_t count = 0;
      while(*str){
		str++;
		count++;
	}
	return count;
}

void string_getNext(char *str, char *dst){
	int i;
	for(i = 0;str[i] != ' ' && str[i] != '\0';i++){
		dst[i] = str[i];
	}
	dst[i] = '\0';
	return;
}

void cut_string(char *str, char *dst, int start_point){
	int i;
	for(i = 0;i < 1024 && str[i] != '\0';i++){
		dst[i] = str[i+start_point];
	}
	dst[i] = 0;
	return;
}

void strcpy(char *copied_str, char *str, int length){
	int i;
	for(i = 0;i < length;i++){
		copied_str[i] = str[i];
	}
	copied_str[i] = '\0';
	return;
}

char *strcat(char *s1, const char *s2) {
	u32_t i = 0;

	while(*s1 != '\0')
		s1++;

	while(s2[i] != '\0'){
		s1[i] = s2[i];
		i++;
	}

	s1[i] = '\0';

	return s1;
}
