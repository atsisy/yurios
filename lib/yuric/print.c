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

	u32_t  i, p = 0;
	u32_t box[78] = { 0 };

	malloc_init();

	char *buffer = malloc(256*sizeof(char));

	stat(fd, box);

	while(1){
		read(fd, buffer, 1);
		for(i = box[2];i < 256; i++, p++ ){
			switch(buffer[i]){
				//改行
			case 0x0a:
			case '\0':
				line[p] = '\0';
				free(buffer);
				seek(fd, i+1, SEEK_CUR);
				return;
			default:
				line[p] = buffer[i];
			}
		}
	}
	
	return;
}
