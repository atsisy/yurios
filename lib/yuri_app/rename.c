#include "../../include/yuri/yuri.h"

int main(int argc, char **argv){
	int fd = open(argv[1], O_RDONLY);
	static char buffer[512];
	read(fd, buffer, 512);
	close(fd);
	int fd2 = open(argv[2], O_CREAT);
	write(fd2, buffer, 512);
	close(fd2);
	exit();
}
