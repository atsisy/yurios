#include "../../include/yuri/yuri.h"

int main(int argc, char **argv);

void __entry__(void){
	malloc_init();
	int argc;
	exit(main(getca(&argc), 0));
}
