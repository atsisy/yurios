#include "../include/kernel.h"
#include "../include/value.h"
#include "../include/sh.h"
#include "../include/string.h"
#include "../include/ata.h"
#include "../include/yrws.h"

void init_yrfs();

/*
 *カウンタのスレッド
 */
void task_b_main(void);

char sent_command[1024];

const struct BOOTINFO *binfo;
struct QUEUE fifo;

unsigned int memtotal;
char *keys0, *keys1;

void Main(void) {

	//以下のメモリ番地はasmhead.nasで記述済み
	//cyls...0xff0 leds...0x0ff1 vmode...0x0ff2 reserve...0x0ff3
	//scrnx...0x0ff4 scrny...0x0ff6 vram.自動..0x0ff8
	//シフト演算は高速化が図れるので積極的に使っていこう
	memman = (struct MEMMAN *)MEMMAN_ADDR;
	binfo  = (struct BOOTINFO *)ADR_BOOTINFO;
	struct QUEUE keycmd;
	struct TIMER *timer, *timer2, *timer3, *timer_ts;

	i32_t fifobuf[128], keycmd_buf[32], *kernel_buf = (i32_t *)memory_alloc(memman, sizeof(i32_t) << 8);

	static char keytable0[0x80] = {
		0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0, 0, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0, 0, ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0x5c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x5c, 0, 0
	};
	static char keytable1[0x80] = {
		0, 0, '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0, 0, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0, 0, '}', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, '_', 0, 0, 0, 0, 0, 0, 0, 0, 0, '|', 0, 0
	};

	keys0 = keytable0;
	keys1 = keytable1;

	struct Process *yuri_kernel, *ylsh;

	init_gdtidt();
	init_pic();
	io_sti(); // IDT/PICの初期化が終わったのでCPUの割り込み禁止を解除

	queue_init(&fifo, 128, fifobuf, 0);
	queue_init(&keycmd, 32, keycmd_buf, 0);

	init_pit();
	io_out8(PIC0_IMR, 0xf8); // PIC1とキーボードを許可(11111001)

	timer    = timer_alloc();
	timer_init(timer, &fifo, 10);
	timer_settime(timer, 1000);


	timer2   = timer_alloc();
	timer_init(timer2, &fifo, 3);
	timer_settime(timer2, 300);


	timer3   = timer_alloc();
	timer_init(timer3, &fifo, 1);
	timer_settime(timer3, 50);

	timer_ts = timer_alloc();
	timer_init(timer_ts, &fifo, 2);
	timer_settime(timer_ts, 2);


	init_keyboard(&fifo, 256);

	init_palette();

	memory_init(memman);
	memtotal = memtest(0x00400000, 0xbfffffff);
	memory_free(memman, 0x00001000, 0x0009e000);
	memory_free(memman, 0x00400000, memtotal-0x00400000);

	yuri_kernel = task_init(memman, "init_process");
	queue_init(&yuri_kernel->irq, 256, kernel_buf, yuri_kernel);
	fifo.task = yuri_kernel;
	task_run(yuri_kernel, 1, 2);

	shell_init();
	INITIALIZE_ATA_DEVICE();
	init_yrfs();

      /*
	 *最初のメッセージを描画
	 */
	puts("Welcome to Yuri.");
	puts("Enjoy hacking on Yuri!!");
	put_char('%');

	ylsh          = task_alloc("shell");
	ylsh->tss.eip = (int) shell_master;
	ylsh->tss.esp = memory_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
	ylsh->tss.es  = 1 * 8;
	ylsh->tss.cs  = 2 * 8;
	ylsh->tss.ss  = 1 * 8;
	ylsh->tss.ds  = 1 * 8;
	ylsh->tss.fs  = 1 * 8;
	ylsh->tss.gs  = 1 * 8;
	//*((int *) (ylsh->tss.esp + 4)) = 0;

	shell_master();

	for(;;){
		task_sleep(task_now());
	}
}

void task_b_main(void){

	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;

	struct QUEUE fifo;
	struct TIMER *timer_ts;
	int i = 0, fifo_buf[128], count = 0;
	char s[11];

	queue_init(&fifo, 128, fifo_buf, 0);
	timer_ts = timer_alloc();
	timer_init(timer_ts, &fifo, 1);
	timer_settime(timer_ts, 2);

	while(1){
		count++;
		if((count % 100) == 0){
			boxfill8(binfo->vram, binfo->scrnx, BLACK, 950, 600, 1024, 800);
			sprintf(s, "%01d", count/100);
			putfonts8_asc(binfo->vram, binfo->scrnx, 950, 600, COL8_FFFFFF, s);
		}
		io_cli();
		if(queue_size(&fifo) == 0){
			io_stihlt();
		}else{
			i = queue_pop(&fifo);
			io_sti();
		}
	}
}
