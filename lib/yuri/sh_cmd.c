#include "../../include/bootpack.h"
#include "../../include/sh.h"
#include "../../include/string.h"
#include "../../include/value.h"

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
	if(indent > MAX_SCROLL){
		scroll(binfo, 16);
	}
	increase_indent();
	sprintf(s, "free:%dKB", memory_total(memman) / 1024);
	print(s);
	return;
}

void command_history(void){
	int i;
	if(indent > MAX_SCROLL){
		scroll((struct BOOTINFO *)ADR_BOOTINFO, 16);
	}

	for(i = 0;i < put_index;i++){
		print(get_history(i));
		if(indent > MAX_SCROLL){
			scroll((struct BOOTINFO *)ADR_BOOTINFO, 16);
		}
		increase_indent();
	}

	indent--;
	return;
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
				print(a);
				increase_indent();
			}
		}
	}
	indent--;
	return;
}

/*
 *=======================================================================================
 *command_cat関数
 *catコマンドを実行する関数
 *=======================================================================================
 */
void command_cat(char *inputed_command){

	int *fat = (int *)memory_alloc_4k(memman, 4 * 2880);
	readfat(fat, (unsigned char *)(ADR_DISKIMG + 0x0000200));

	int x, y;
	struct FileInfo *finfo = (struct FileInfo *)(ADR_DISKIMG + 0x002600);
	char s[30], *p;

	for(y = 0;y < 11;y++){	//とりあえず初期化
			s[y] = ' ';
	}
	y = 0;
	for(x = 4;y < 11 && inputed_command[x] != 0;x++){
		if(inputed_command[x] == '.' && y <= 8){
			y = 8;
		}else{
			s[y] = inputed_command[x];
			if('a' <= s[y] && s[y] <= 'z'){	//小文字は大文字に直す
				s[y] -= 0x20;
			}
			y++;
		}
	}
	//ファイルを探す
	for(x = 0;x < 224;){
		if(finfo[x].name[0] == 0x00){
			break;
		}
		if((finfo[x].type & 0x18) == 0){
			for(y = 0;y < 11;y++){
				if(finfo[x].name[y] != s[y]){
					goto type_next_file;
				}
			}
			break;
		}
type_next_file:
		x++;
	}
	if(x < 224 && finfo[x].name[0] != 0x00){
		//ファイルが見つかった時
		p = (char *)memory_alloc_4k(memman, finfo[x].size);
		loadfile(finfo[x].clustno, finfo[x].size, p, fat, (char *)(ADR_DISKIMG + 0x003e00));
		for(y = 0;y < finfo[x].size;y++){
			s[0] = p[y];
			s[1] = '\0';
			if(length > (binfo->scrnx-166)){
				//increase_indent();
			}
			if(s[0] == 0x09){	//タブだった場合
				while(length % 4 == 0){	//4の倍数分の空白を開ける
					print(" ");
					continue;
				}
			}else if(s[0] == 0x0a){	//改行がだった場合
				increase_indent();
				continue;
			}else if(s[0] == 0x0d){
				length = 0;
				continue;
			}
			print(s);
			increase_length();
		}
		memory_free_4k(memman, (int)p, finfo[x].size);
	}else{
		//ファイルが見つからなかった場合
		print("File not found.");
	}
}
