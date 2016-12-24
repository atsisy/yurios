#include "../../include/kernel.h"
#include "../../include/sh.h"
#include "../../include/string.h"
#include "../../include/value.h"
#include "../../include/yrfs.h"

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
void command_echo(char *inputed_command){
	/*
	 *文字列を表示するべき文字列だけにカット
	 */
	char str[1024];
	cut_string(inputed_command, str, 5);

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
		scroll(binfo, 16);

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
		print(str); //改行はすでに入っている
	}else{
		puts("ERROR");
	}

	//開放
	memory_free(memman, str, 1024);
}

/*
 *=======================================================================================
 *command_ls関数
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
