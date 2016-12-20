#include "../../include/yuri/yuri.h"
#include "../../include/yuri/stdio.h"

int main(void){

      int fd = 0;
	malloc_init();
	char *str = (char *)malloc(256);
	char *wr = (char *)malloc(256);
	int i;
	for(i = 0;i < 256;i++)
		wr[i] = 0;
	wr[0] = 'Y';
	wr[1] = 'U';
	wr[2] = 'R';
	wr[3] = 'I';
	wr[4] = '\n';
	wr[5] = 'O';
	wr[6] = 'S';

	fd = open("b.txt", O_CREAT);
	//fd = open("b.txt", O_RDONLY);

	write(fd, wr, 256);

	getline(fd, str);
	puts(str);
	getline(fd, str);
	puts(str);
	getline(fd, str);
	puts(str);

	close(fd);

	exit();
}
