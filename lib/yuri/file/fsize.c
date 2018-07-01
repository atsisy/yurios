#include "../../../include/mm.h"
#include "../../../include/sh.h"

/*
 *=======================================================================================
 *GetFlpFileSize関数
 *メモリ上にマップされたFAT内のファイルサイズを取得する関数
 *=======================================================================================
 */
size_t GetFlpFileSize(char *FileName){
	int *fat = (int *)memory_alloc_4k(memman, 4 * 2880);
	readfat(fat, (unsigned char *)(ADR_DISKIMG + 0x0000200));

	int x, y = 0;
	struct FileInfo *finfo = (struct FileInfo *)(ADR_DISKIMG + 0x002600);
	char s[30];

	zeroclear_8array(s, 30);

	for(x = 0;y < 11 && FileName[x] != 0;x++){
		if(FileName[x] == '.' && y <= 8){
			y = 8;
		}else{
			s[y] = FileName[x];
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
		//ファイルが見つかった
	      return finfo[x].size;
	}

	//ファイルが見つからなかった
	return 0;
}
