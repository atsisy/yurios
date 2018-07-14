#include <mm.h>
#include "../include/value.h"
#include "../include/sh.h"
#include "../include/string.h"
#include "../include/ata.h"
#include "../include/util_macro.h"

void init_yrfs();
static void init();

/*
 *カウンタのスレッド
 */
void task_b_main(void);

char sent_command[1024];

const struct BOOTINFO *binfo;
struct QUEUE fifo;

unsigned int memtotal;
char *keys0, *keys1;

void Main(void)
{
	//以下のメモリ番地はasmhead.nasで記述済み
	//cyls...0xff0 leds...0x0ff1 vmode...0x0ff2 reserve...0x0ff3
	//scrnx...0x0ff4 scrny...0x0ff6 vram.自動..0x0ff8
	//シフト演算は高速化が図れるので積極的に使っていこう
	memman = (struct MEMMAN *)MEMMAN_ADDR;
	binfo  = (struct BOOTINFO *)ADR_BOOTINFO;
	struct QUEUE keycmd;
	struct TIMER *timer, *timer2, *timer3, *timer_ts;
	
	i32_t fifobuf[128], keycmd_buf[32], *kernel_buf = (i32_t *)memory_alloc(memman, sizeof(i32_t) << 8);

/*
	keys0 = keytable0;
	keys1 = keytable1;
*/
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

	memory_init(memman);
	memtotal = memtest(0x00400000, 0xbfffffff);
	memory_free(memman, 0x00001000, 0x0009e000);
	memory_free(memman, 0x00400000, memtotal-0x00400000);        
        
        u8_t result = init_virtual_memory_management();
        char *p = (char *)(0x10000000);
        *p = 0;
        puts("I'm back!");

        memcpy(p, "unko", 3);
        p[4] = 0;

        puts(p);

        heap_init(0x11000000, MM_PAGE_SIZE << 6);
        char *buf = (char *)kr_kmalloc(10);
        printk("returned addr: 0x%x\n", (u32_t)buf);
        memcpy(buf, "unko", 3);
        buf[4] = 0;
        puts(buf);
        kr_kfree(buf);
        puts("fin");
        
        while(1)
                io_hlt();

        if(result == MM_OK){
                put_character('s', 0x07, 0, 0, 0);
        }else{
                put_character('f', 0x07, 0, 0, 0);
        }
        
        yksh_init();
        
	yuri_kernel = task_init(memman, "yuri kernel");
	yuri_kernel->irq = (struct QUEUE *)memory_alloc(memman, sizeof(struct QUEUE));
	queue_init(yuri_kernel->irq, 256, kernel_buf, yuri_kernel);
	fifo.task = yuri_kernel;
	task_run(yuri_kernel, 1, 2);

	//副作用的な初期化関数を一度に実行
	init();

      /*
	 *最初のメッセージを描画
	 */
	puts("Welcome to Yuri.");
	puts("Enjoy hacking on Yuri!!");

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
			sprintf(s, "%01d", count/100);
		}
		io_cli();
		if(IS_FAILURE(queue_size(&fifo))){
			io_stihlt();
		}else{
			i = queue_pop(&fifo);
			io_sti();
		}
	}
}

static void init(){
	InitStreams();
	INITIALIZE_ATA_DEVICE();
	init_yrfs();
}
