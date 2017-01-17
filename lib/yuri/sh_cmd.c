#include "../../include/kernel.h"
#include "../../include/sh.h"
#include "../../include/string.h"
#include "../../include/value.h"
#include "../../include/yrfs.h"
#include "../../include/ata.h"
#include "../../include/sysc.h"

u8_t *read_yim(char *file_name, char *buffer, u32_t length);

/*
 *シェル組み込みのコマンドだけを書いていきたいなと思ふ
 */

/*
 *=======================================================================================
 *command_echo関数
 *シェル上のコマンド”echo”に対応する処理を行う関数
 *引数
 *char *inputed_command
 *=>シェルに入力されたそのままの文字列
 *=======================================================================================
 */
void command_print(char *inputed_command){
	/*
	 *文字列を表示するべき文字列だけにカット
	 */
	char str[1024];
	cut_string(inputed_command, str, 6);

	/*
	 *カットした文字列を表示
	 */
	print(str);

	indent_shell();

	return;
}

/*
 *=======================================================================================
 *command_memory関数
 *現在のメモリ状況を標準出力する関数
 *memoryコマンドに対応する処理を行う関数
 *引数返り値なし
 *=======================================================================================
 */
void command_memory(void){
	char s[20];
	sprintf(s, "total:%dMB", memtotal / (1024 * 1024));
	print(s);

	if(indent > MAX_SCROLL)
		scroll(16);

	indent_shell();
	sprintf(s, "free:%dKB", memory_total(memman) / 1024);
	print(s);

	indent_shell();

	return;
}

void command_history(void){
	//ファイルディスクリプタ
	i32_t fd;

	//文字列用メモリ確保
	char *str = (char *)memory_alloc(memman, 1024);

	if((fd = do_open("history", __O_RDONLY__)) != -1){
		do_read(fd, str, 2);
		puts(str);
	}else{
		puts("ERROR");
	}

	//開放
	memory_free(memman, (u32_t)str, 1024);
}

/*
 *=======================================================================================
 *command_ls関数inode.begin_address.sector+i
 *lsコマンドを実行する関数
 *=======================================================================================
 */
void command_ls(void){
	struct FileInfo *finfo = (struct FileInfo *)(ADR_DISKIMG + 0x002600);
	int x, y;
	char a[30];
	for(x = 0;x < 224;x++){
		if(finfo[x].name[0] == 0x00){	//0x00はこれ以降ファイルが存在しないことを表す
			break;
		}
		if(finfo[x].name[0] != 0x05){	//0x05はこのファイルが削除されたことを示す
			if((finfo[x].type & 0x18) == 0){
				sprintf(a, "filename.ext  %7d", finfo[x].size);
				for(y = 0;y < 8;y++){	//ここから順に置き換えていく
					a[y] = finfo[x].name[y];
				}
				a[9] = finfo[x].ext[0];
				a[10] = finfo[x].ext[1];
				a[11] = finfo[x].ext[2];
				a[12] = '\0';
				puts(a);
			}
		}
	}

	return;
}

/*
 *=======================================================================================
 *command_cat関数
 *catコマンドを実行する関数
 *=======================================================================================
 */
void command_cat(char *inputed_command){

	/*
	*メモリ確保
	*/
	char *src = (char *)memory_alloc(memman, 200000);
	
	/*
	*読み込み
	*/
	read_mem2hd(inputed_command, src, 256);
	
	puts(src);
	
	/*
	*開放
	*/
	memory_free(memman, (u32_t)src, 256);

}

/*
 *=======================================================================================
 *command_lscpu関数
 *lscpuコマンドを実行する関数
 *=======================================================================================
 */
void command_lscpu(void) {
	u32_t ebx, ecx, edx;
	char vendor[13];
	ebx = ecx = edx = 0;

	/*
	*CPUベンダ名を取得
	*/
	cpu_vendor(&ebx, &ecx, &edx);
	print("Vendor ID:");

	vendor[0] = (char)ebx;
	vendor[1] = (char)(ebx >> 8);
	vendor[2] = (char)(ebx >> 16);
	vendor[3] = (char)(ebx >> 24);
	vendor[4] = (char)edx;
	vendor[5] = (char)(edx >> 8);
	vendor[6] = (char)(edx >> 16);
	vendor[7] = (char)(edx >> 24);
	vendor[8] = (char)ecx;
	vendor[9] = (char)(ecx >> 8);
	vendor[10] = (char)(ecx >> 16);
	vendor[11] = (char)(ecx >> 24);
	vendor[12] = '\0';;

	puts(vendor);

	/*
	*CPUのキャッシュ情報を得る
	*/
	cpu_cache_sub(&ebx, &ecx, &edx);
	puts("CPU cache infomation.");

	sprintf(vendor, "L1 cache:%d", ebx);
	puts(vendor);

	sprintf(vendor, "L2 cache:%d", ecx);
	puts(vendor);

	sprintf(vendor, "L3 cache:%d", edx);
	puts(vendor);

	/*
	//ブランドストリング
	zeroclear_8array(vendor, 13);
	cpu_brand_string(vendor);

	puts(vendor);
	*/
}

/*
 *=======================================================================================
 *command_show関数
 *showコマンドを実行する関数
 *=======================================================================================
 */
void command_show(char *inputed_command) {
	/*
 	 *文字列を表示するべき文字列だけにカット
 	 */
 	char fname[256], str[1024];
	int fd;
 	cut_string(inputed_command, fname, 5);

	if((fd = do_open(fname, __O_RDONLY__)) != -1){
		do_read(fd, str, 2);
		puts(str);
	}else{
		puts("file not found.");
	}

 	return;
}


/*
 *=======================================================================================
 *command_wirteyim関数
 *yim画像をHDDに書き込むコマンドの内部関数
 *=======================================================================================
 */
void command_writeyim(char *file_name){
	
	u32_t size = fat_getsize(file_name);
	char *src = (char *)memory_alloc_4k(memman, size);
	char *p = src;
	
	/*
	 *読み込み
	 */
	struct i_node inode;
	u32_t i;
	read_yim(file_name, src, 256);

	i32_t fd = do_open(file_name, __O_CREAT__);
	do_write(fd, src, size);
	iread(&inode, fd);
	for(i = 0;i < byte2sectors(size);i++){
		write_ata_sector(&ATA_DEVICE0, inode.begin_address.sector+i, src, 1);
		blocks_info[inode.begin_address.sector+i].exist = __USED_BLOCK__;
		src += 512;
	}

	memory_free_4k(memman, (u32_t)p, size);
			
}

/*
 *=======================================================================================
 *command_rm関数
 *ファイルを削除するコマンドの内部関数
 *=======================================================================================
 */
void command_rm(char *file_name){
      RemoveFile(file_name);
}

/*
 *=======================================================================================
 *command_touch関数
 *ファイルを作成するコマンドの内部関数
 *=======================================================================================
 */
void command_touch(char *file_name){
	/*
	 *作成
	 */
	i32_t fd = do_open(file_name, __O_CREAT__);

	/*
	 *クローズ
	 */
	do_close(fd);
}

/*
 *=======================================================================================
 *command_cp関数
 *ファイルをコピーするコマンドの内部関数
 *=======================================================================================
 */
void command_cp(int argc, char **argv){
	/*
	 *ファイル名をコピー
	 */
	char new_filename[64];
	strcpy(new_filename, argv[2]);

	/*
	 *ファイル用のバッファを確保
	 */
	char *buffer = (char *)memory_alloc(memman, 512);

	//オープン
	int fd = do_open(argv[1], __O_RDONLY__);
	//読み込み
	do_read(fd, buffer, 512);

	//ファイル作成
	int fd2 = do_open(new_filename, __O_CREAT__);
	//書き込み
	do_write(fd2, buffer, 512);

	//クローズ
	do_close(fd);
	do_close(fd2);

	//親プロセスに終了したことを知らせる
	struct Process *me = task_now();
	kernel_send(me->parent, __SIG_KILL_ME__);

	/*
	 *キルされるのを待つ
	 */
	end();
}

/*
 *=======================================================================================
 *fae関数
 *プロセスの生成、起動を行う関数
 *=======================================================================================
 */
i32_t fae(i32_t function, u32_t argc, char *command, u32_t flag){

	i32_t i;

	char **argv = !command ? extend(command) : NULL;
	struct Process *parent = task_now();

	struct Process *child;
	child = task_alloc(parent->proc_name);
	i32_t *buf = (i32_t *)memory_alloc(memman, 40);
	queue_init(child->irq, 10, buf, NULL);

	child->tss.esp = memory_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
	child->tss.es = 1 * 8;
	child->tss.cs = 2 * 8;
	child->tss.ss = 1 * 8;
	child->tss.ds = 1 * 8;
	child->tss.fs = 1 * 8;
	child->tss.gs = 1 * 8;
	child->tss.eip = function;

	*((char ***)(child->tss.esp + 8)) = argv;
	*((u32_t *)(child->tss.esp + 4)) = argc;

	child->parent = parent;

	task_run(child, 3, 2);

	for(;;){
		break;
		if(!queue_size(parent->irq)){
			task_sleep(parent);
			io_sti();
		}else{
			i = queue_pop(parent->irq);
			if(i == __SIG_KILL_ME__){
				break;
			}
		}
	}

	task_remove(child);

	return 0;

}


char **extend(char *line){
	i32_t space_count = 1, i, n;
	char **argv;
	
	for(i = 0;line[i] != '\0';i++){
		if(line[i] == ' '){
			space_count++;
		}
	}

	argv = (char **)memory_alloc(memman, sizeof(char *) * space_count);

	for(n = 0;n < space_count;n++){
		for(i = 0;line[i] != ' ';){
			i++;
		}
		argv[n] = (char *)memory_alloc(memman, i+1);
		
		string_getNext(line, argv[n]);
		line += i+1;
	}

	return argv;
}
