#include "../../../include/yrfs.h"
#include "../../../include/sh.h"
#include "../../../include/kernel.h"

/*
 *=======================================================================================
 *メモリ上のファイルからHDDに書き込む関数
 *=======================================================================================
 */
u32_t write_mem2hd(char *file_name){
	int *fat = (int *)memory_alloc_4k(memman, 4 * 2880);
	readfat(fat, (unsigned char *)(ADR_DISKIMG + 0x0000200));

	int x, y;
	struct FileInfo *finfo = (struct FileInfo *)(ADR_DISKIMG + 0x002600);
	char s[30], *p;

	for(y = 0;y < 11;y++){	//とりあえず初期化
			s[y] = ' ';
	}
	y = 0;
	for(x = 4;y < 11 && file_name[x] != 0;x++){
		if(file_name[x] == '.' && y <= 8){
			y = 8;
		}else{
			s[y] = file_name[x];
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
				indent_shell();
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

	indent_shell();
}
