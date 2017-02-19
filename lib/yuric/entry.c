#define __YURI_CMDLINE_BUFFER_SIZE__ 256
typedef int i32_t;
void malloc_init();
i32_t getca(int *argc, char *argv);
void exit(i32_t status);
int main(int argc, char *argv);

void __entry__(void){
	malloc_init();
	int argc;
	char argv[__YURI_CMDLINE_BUFFER_SIZE__];
	getca(&argc, argv);
	exit(main(argc, argv));
}
