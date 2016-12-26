/*
 *=======================================================================================
 *yuri script
 *=======================================================================================
 */
#include "../../../include/yuri/yuri.h"
#include "../../../include/yuri/stdio.h"
#include "../../../include/yuri/string.h"

void print(char *code);

int main(void) {
	static char code[1024];
	static char identifier[24];

	while(1){
		read(stdin, code, 1024);

		string_getNext(code, identifier);

		if(strcmp(identifier, "quit")){
			break;
		}else if(strcmp(identifier, "print")){
			print(code);
		}else{
			puts("Unknown identifier.");
		}
		
	}

	exit();
}

void print(char *code){
	static char arg[256];
	cut_string(code, arg, 5);
	puts(arg);
}