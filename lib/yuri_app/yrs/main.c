/*
 *=======================================================================================
 *yuri script
 *=======================================================================================
 */
#include "../../../include/yuri/yuri.h"
#include "../../../include/yuri/stdio.h"

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

void string_getNext(char *str, char *dst){
	int i;
	for(i = 0;str[i] != ' ' && str[i] != '\0';i++){
		dst[i] = str[i];
	}
	dst[i] = '\0';
	return;
}

int main(void) {
	char code[1024];
	char identifier[24];

	while(1){
		read(stdin, code, 1024);

		string_getNext(code, identifier);

		if(strcmp(identifier, "quit")){
			break;
		}else if(strcmp(identifier, "print")){
			puts(identifier);
		}else{
			puts("Unknown identifier.");
		}
		
	}

	exit();
}
