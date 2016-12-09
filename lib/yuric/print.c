#include "../../include/yuri/yuri.h"

/*
 *=======================================================================================
 *puts関数
 *文字列を表示する関数
 *=======================================================================================
 */
void puts(char *string){
	while(*string) {
		write(stdout, string, 1);
		string++;
	}
	newline();
}

/*
 *=======================================================================================
 *getline関数
 *改行で区切ってファイルの中身を返す
 *=======================================================================================
 */
void getline(int fd, char *line) {
	
	u32_t  i, p;

	malloc_init();

	u32_t *box = (u32_t *)malloc(78*sizeof(u32_t));
	char *buffer = (char *)malloc(256*sizeof(char));

	p = 0;

	stat(fd, box);

	while(1){
		read(fd, buffer, 1);
		for(i = box[2];i < 256; i++, p++ ){
			switch(buffer[i]){
				//改行
			case 0x0a:
			case '\0':
				line[p] = '\0';
				free(box);
				free(buffer);
				seek(fd, i+1, SEEK_CUR);
				return;
			default:
				line[p] = buffer[i];
			}
		}
	}
	
}
