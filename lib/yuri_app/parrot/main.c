#include "../../../include/yuri/yuri.h"
#include "../../../include/yuri/stdio.h"

int main(int argc, char **argv){

	puts(argv);

	char *a = malloc(128);

	read(stdin, a, 127);

	write(stdout, a, 127);

	newline();

	return 0;
}
