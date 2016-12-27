/*
 *=======================================================================================
 *yuri script
 *=======================================================================================
 */
#include "../../../include/yuri/yuri.h"
#include "../../../include/yuri/stdio.h"
#include "../../../include/yuri/string.h"

void print_sc(char *code);

int main(void) {
	static char code[1024];
	static char identifier[24];

	static int times = 0;
	static char times_flag = 0;

	while(1){
		read(stdin, code, 1024);

		string_getNext(code, identifier);

		if(strcmp(identifier, "quit")){
			break;
		}else if(strcmp(identifier, "print")){
			if(times_flag){
				while(times){
					print_sc(code);
					times--;
				}
			}else{
				print_sc(code);
			}
		}else{
			puts("Unknown identifier.");
		}
		
	}

	exit();
}

void print_sc(char *code){
	code += 6;
	puts(code);
}