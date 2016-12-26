/*
 *=======================================================================================
 *yuri script
 *=======================================================================================
 */
#include "../../../include/yuri/yuri.h"
#include "../../../include/yuri/stdio.h"
#include "../../../include/yuri/string.h"

int main(void) {
	static char code[1024];
	static char identifier[24];
	static char arg[256];

	while(1){
		read(stdin, code, 1024);

		string_getNext(code, identifier);

		if(strcmp(identifier, "quit")){
			break;
		}else if(strcmp(identifier, "print")){
			cut_string(code, arg, 5);
			puts(arg);
		}else{
			puts("Unknown identifier.");
		}
		
	}

	exit();
}
