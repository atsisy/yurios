#include "../../include/yuri/yuri.h"
#include "../../include/yuri/stdio.h"

void getline(int fd, char *line);

int main(void){

	unsigned int fd = -1;
	char str[256];
	char wr[256] = "YURI\nOS";

	fd = open("b.txt", O_CREAT);
	write(fd, wr, 1);
	//read(fd, str, 1);
	getline(fd, str);
	puts(str);
	getline(fd, str);
	puts(str);
	getline(fd, str);
	puts(str);

	exit();
}
