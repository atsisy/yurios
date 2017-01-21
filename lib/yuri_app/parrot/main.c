#include "../../../include/yuri/yuri.h"
#include "../../../include/yuri/stdio.h"

void main(void){

	malloc_init();
	char *a = malloc(128);

	read(stdin, a, 127);

	write(stdout, a, 127);

	newline();

      exit();

}
