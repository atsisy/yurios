#include "../../include/yuri/yuri.h"
#include "../../include/yuri/stdio.h"

/*
 *=======================================================================================
 *getline関数
 *改行で区切ってファイルの中身を返す
 *=======================================================================================
 */
void getline(int fd, char *line) {
	u32_t  i, p = 0;
	
	malloc_init();
	char *buffer = malloc(256*sizeof(char));

	while(1){
		read(fd, buffer, 1);
		for(i = 0;i < 256; i++, p++ ){
			switch(buffer[i]){
			//改行
			case 0x0a:
				line[p] = '\0';
				free(buffer);
				return;
			case '\0':
				free(buffer);
				return;
			default:
				line[p] = buffer[i];
			}
		}
	}

}
