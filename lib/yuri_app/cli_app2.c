#include "../../include/yuri/yuri.h"
#include "../../include/yuri/stdio.h"

void p();

int main(void){

      int fd = 0;
	char str[256];
	char wr[256] = "YURI\nOS";
	
	fd = open("b.txt", O_CREAT);
	
	write(fd, wr, 1);

	getline(fd, str);
	puts(str);
	getline(fd, str);
	puts(str);
	getline(fd, str);
	puts(str);
	
	close(fd);
	
	exit();

}
