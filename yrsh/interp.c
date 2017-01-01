#include "../include/sh.h"
#include "../include/string.h"
#include "../include/ata.h"
#include "../include/kernel.h"
#include "../include/yrfs.h"

int do_open(char *pathname, u32_t flags);
i32_t fae(i32_t function, char *command, u32_t flag);
void command_cp(char *command);

void yrsw_main();
void getline(char *all, char *line);
void gline(int fd, char *line);

void yrsh_interpreter(char *command){

      int *fat = (int *)memory_alloc_4k(memman, 4 * 2880);
	readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));
      char *part = (char *)memory_alloc(memman, 1024);
      string_getNext(command, part);	//受け取ったコマンドを分割して先頭の文字列のみを取り出す

            /*
		 *コマンドに合わせて処理を実行する
		 */
		if(strcmp(part, "memory")){
			/*
			 *空きメモリ容量を表示するコマンド
			 */
			command_memory();
		}else if(strcmp(part, "print")){
			/*
			 *UNIXコマンドのechoコマンド的なやつ
			 */
			command_print(command);
		}else if(strcmp(part, "history")){
			/*
			 *そのまんまhistoryコマンド
			 */
			command_history();
		}else if(strcmp(part, "ls")){
			/*
			 *lsもどき
			 */
			command_ls();
		}else if(strcmp(part, "cat")){
			/*
			 *ファイルの内容を表示
			 */
			command_cat(command);
			
		}else if(strcmp(command, "clear")){
			/*
			 *シェルの画面を新しい画面に置き換えるコマンド
			 *UNIXのclearと同じく
			 */
			ylsh_clear();
		}else if(strcmp(command, "open")){

			struct i_node inode;
			int fd = do_open("yuri_doc.txt", __O_CREAT__);
			iread(&inode, fd);
			
			char *src = (char *)memory_alloc(memman, 256);
			
			read_mem2hd("cat YURI_DOC.TXT", src, 256);
			
			do_write(fd, src, 1);
			
			memory_free(memman, (u32_t)src, 256);

			fd = do_open("yuri_doc.txt", __O_RDONLY__);
			fadd(fd, "\nSATORI");
			/*
			  do_read(fd, src2, 1);

			  puts(src2);
			*/
		}else if(strcmp(command, "fszeroclear")){
			/*
			  u32_t i;
			  u8_t zero[512] = { 0 };
			  for(i = 0;i < __BLOCKS_LIMIT__;i++)
			  write_ata_sector(&ATA_DEVICE0, i, zero, 1);
			
			  for(i = 0;i < __INODE_LIMIT__;i++)
			  blocks_info[i].exist = __UNUSED_BLOCK__;
			*/
			filesystem_zeroclear();
		}else if(strcmp(command, "date")){
			char time[32];
			sprintf(time, "%d:%d %d/%d %d", do_gettime(__HOUR__), do_gettime(__MINUTE__), do_gettime(__MONTH__),
				  do_gettime(__DAY__), do_gettime(__YEAR__));
			puts(time);
		}else if(strcmp(command, "fls")){
			file_list(NULL);
		}else if(strcmp(command, "lscpu")){
			command_lscpu();
		}else if(strcmp(command, "fork")){
			/*
			if(do_fork() == 0){
				puts("child");
			}else{
				puts("parent");
			}
			*/
		}else if(strcmp(command, "ps")){
			extern struct PROCESS_MASTER *process_master;
			u32_t i;
			char p_info[36];
			for(i = 0;i < MAX_TASKS;i++){
				if(process_master->ALL_PROCESSES[i].status == RUNNING_PROCESS){
					zeroclear_8array(p_info, 36);
					sprintf(p_info, "PID:%d %s", process_master->ALL_PROCESSES[i].pid, process_master->ALL_PROCESSES[i].proc_name);
					puts(p_info);
				}
			}
		}else if(strcmp(part, "show")){
			command_show(command);
		}else if(strcmp(part, "cp")){
			fae((i32_t)command_cp, command, 0);
			puts("p end");
		}else if(strcmp(command, "yrws")){
			yrsw_main();
		}else if(strcmp(command, "end")){
			puts("finish");
		}else if(strcmp(part, "writeyim")){
			char file_name[256];
			cut_string(command, file_name, 9);
			command_writeyim(file_name);
		}else if(strcmp(command, "os")){
			puts("YuriOS Version 0.1.0b\nRelease in x/x, 201x");
		}else if(strcmp(part, "rm")){
			/*
			 *=======================================================================================
			 *rmコマンド
			 *=======================================================================================
			 */
			char file_name[128];
			cut_string(command, file_name, 3);

			command_rm(file_name);
		}else if(strcmp(part, "touch")){
			char file_name[128];
			cut_string(command, file_name, 6);

			command_touch(file_name);
		}else if(strcmp(part, "yrs")){


			char file_name[36];
			char n[36] = { 0 };
			cut_string(command, file_name, 4);
			int fd;
			fd = do_open(file_name, __O_RDONLY__);
			gline(fd, n);

			while(!strcmp(n, "end")){
				yrsh_interpreter(n);
				zeroclear_8array(n, 32);
				gline(fd, n);
			}


			do_close(fd);

		}else if(do_shell_app(fat, command) == 0){
			//対応するコマンドではなく、さらにアプリケーションでもない場合
			/*
			 *外部アプリケーション実行
			 *もしくはそんなコマンドねーよ的な
			 */
			print("Sorry. I Could not recognize '");
			print(command);
			puts("'.");
		}

            memory_free_4k(memman, (u32_t)fat, 4 * 2880);
            memory_free(memman, (u32_t)part, 1024);
}

/*
 *=======================================================================================
 *getline関数
 *改行で区切ってファイルの中身を返す
 *=======================================================================================
 */
void getline(char *all, char *line) {

	while(*all != '\n' && *all != '\0'){
		*line = *all;
		line++;
		all++;
	}

}

/*
 *=======================================================================================
 *getline関数
 *改行で区切ってファイルの中身を返す
 *=======================================================================================
 */
void gline(int fd, char *line) {

	u32_t  i, p;
	u32_t *box = (u32_t *)memory_alloc(memman, sizeof(u32_t) * 128);
	char *buffer = (char *)memory_alloc(memman, 1024);

	p = 0;

	do_stat(fd, box);
	i = box[4];

	//print_value(i, 500, 100+(i << 2));

	while(1){
		do_read(fd, buffer, 512);
		for(;i < 512; i++, p++ ){
			switch(buffer[i]){
				//改行
			case 0x0a:
			case '\0':
				line[p] = '\0';
				do_seek(fd, i+1, __SEEK_SET__);

				memory_free(memman, (u32_t)box, sizeof(u32_t) * 128);
				memory_free(memman, (u32_t)buffer, 1024);
				return;
			default:
				line[p] = buffer[i];
			}
		}
	}

}
