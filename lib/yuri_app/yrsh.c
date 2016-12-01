#include "../../include/yuri.h"

int main(void){
	char command[256];
	
	write(stdout, "Welcome to Yuri Operating Systems", 100);

	while(1){
		read(stdin, command, 256);
	}

	exit();
}
