#include "../../include/yuri/yuri.h"
#include "../../include/yuri/stdio.h"

int main(void){

      int fd = 0;
	char str[256];
	char wr[256] = "YURI\nOS";

	fd = open("b.txt", O_CREAT | O_RDONLY);

	write(fd, wr, 256);

	getline(fd, str);
	puts(str);
	getline(fd, str);
	puts(str);
	getline(fd, str);
	puts(str);

	close(fd);

	exit(0);
}
