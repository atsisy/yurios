#include "../../include/yuri/yuri.h"

int main(int argc, char **argv);

void __entry__(void){
	malloc_init();
	int argc;
	char argv[256];
	getca(&argc, argv);
	puts(argv);
	exit(main(argc, argv));
}