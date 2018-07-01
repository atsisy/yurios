#include "../../../include/yrfs.h"
#include "../../../include/sh.h"
#include "../../../include/kernel.h"
#include "../../../include/string.h"

/*
 *=======================================================================================
 *メモリ上のファイルからHDDに書き込む関数
 *=======================================================================================
 */
size_t read_mem2hd(char *file_name, char *buffer, u32_t length){
	int *fat = (int *)memory_alloc_4k(memman, 4 * 2880);
	readfat(fat, (unsigned char *)(ADR_DISKIMG + 0x0000200));

	int x, y;
	struct FileInfo *finfo = (struct FileInfo *)(ADR_DISKIMG + 0x002600);
	char s[30], *p;

	for(y = 0;y < 11;y++){	//とりあえず初期化
			s[y] = ' ';
	}
	y = 0;
	for(x = 0;y < 11 && file_name[x] != 0;x++){
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
		printk("File not found.");
	}

	return finfo[x].size;
}

u8_t *read_yim(char *file_name, u8_t *buffer, u32_t length){
	int *fat = (int *)memory_alloc_4k(memman, 4 * 2880);
	readfat(fat, (unsigned char *)(ADR_DISKIMG + 0x0000200));

	u32_t x, y;
	struct FileInfo *finfo = (struct FileInfo *)(ADR_DISKIMG + 0x002600);
	char s[30], *p;

	for(y = 0;y < 11;y++){	//とりあえず初期化
			s[y] = ' ';
	}
	y = 0;
	for(x = 0;y < 11 && file_name[x] != 0;x++){
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
   
		p = (char *)(ADR_DISKIMG + 0x003e00);
		p += (finfo[x].clustno*512);
		
		u32_t x, y, width, height;

		/*
		 *高さと幅を取得
		 */
		uchar4tou32((u8_t *)&p[3], &width);
		uchar4tou32((u8_t *)&p[7], &height);

		/*
		 *画像データをコピ〜
		 */
		/*
		for(i = 0;i < (width * height)+11;i++){
			buffer[i] = p[i];
		}
		*/
		for(y = 0;y < height;y++){
			for(x = 0;x < width;x++){
				buffer[y*width+x] = p[y*width+x];
			}
		}

		//デバッグ用（画像を直に表示）
		/*=============================================================================
		for(y = 0;y < height;y++){
			for(x = 0;x < width;x++){
				binfo->vram[(y * binfo->scrnx) + x] = p[(y * width) + x+11];
			}
		}
		==============================================================================*/

	}else{
		//ファイルが見つからなかった場合
		printk("File not found.");
	}

	return buffer;
}

u8_t *read_elf(char *file_name, u8_t *buffer, u32_t length){
	int *fat = (int *)memory_alloc_4k(memman, 4 * 2880);
	readfat(fat, (unsigned char *)(ADR_DISKIMG + 0x0000200));

	u32_t x, y;
	struct FileInfo *finfo = (struct FileInfo *)(ADR_DISKIMG + 0x002600);
	char s[30], *p;

	for(y = 0;y < 11;y++){	//とりあえず初期化
			s[y] = ' ';
	}
        
	y = 0;
	for(x = 0;y < 11 && file_name[x] != 0;x++){
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
   
		p = (char *)(ADR_DISKIMG + 0x003e00);
		p += (finfo[x].clustno*512);

		
	      u32_t i;

		for(i = 0;i < length;i++){
			buffer[i] = p[i];
		}

	}else{
		//ファイルが見つからなかった場合
		printk("File not found.");
	}

	return buffer;
}
