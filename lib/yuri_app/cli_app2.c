#include "../../include/yuri.h"

int main(void){
      unsigned int fd = -1;
	fd = open("a.txt", O_CREAT);
	if((fd >> 24) == 20)
		write(stdout, "opened", 6);
	exit();
}
