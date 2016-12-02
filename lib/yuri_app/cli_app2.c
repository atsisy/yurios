#include "../../include/yuri.h"

int main(void){

	unsigned int fd = -1;
	malloc_init();
	char str[5];
	char wr[5] = "YURI";
	str[0] = 'A';

	fd = open("a.txt", O_CREAT);
	write(fd, wr, 5);
	read(fd, str, 5);
	write(stdout, str, 5);

	exit();
}
