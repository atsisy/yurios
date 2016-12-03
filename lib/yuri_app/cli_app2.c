#include "../../include/yuri/yuri.h"
#include "../../include/yuri/stdio.h"

int main(void){

	unsigned int fd = -1;
	malloc_init();
	char str[5];
	char wr[5] = "YURI";
	str[0] = 'A';

	fd = open("a.txt", O_CREAT);
	write(fd, wr, 5);
	read(fd, str, 5);

	puts(str);

	unsigned int fd2 = 0;
	char wr2[5] = "ANNE";
	fd2 = open("b.dat", O_CREAT);
	write(fd2, wr2, 5);
	read(fd2, str, 5);

	puts(str);

	exit();
}
