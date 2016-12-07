/*
 *=======================================================================================
 *yuri script
 *=======================================================================================
 */
#include "../../../include/yuri/yuri.h"
#include "../../../include/yuri/stdio.h"
#include "../../../include/yuri/string.h"

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
