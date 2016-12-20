#include "../../../include/yrfs.h"
#include "../../../include/sh.h"
#include "../../../include/kernel.h"
#include "../../../include/string.h"

/*
 *=======================================================================================
 *メモリ上のファイルからHDDに書き込む関数
 *=======================================================================================
 */
char *read_mem2hd(char *file_name, char *buffer, u32_t length){
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
			*buffer = p[y];
			buffer++;
		}
		memory_free_4k(memman, (int)p, finfo[x].size);
	}else{
		//ファイルが見つからなかった場合
		print("File not found.");
	}

	return buffer;
}
