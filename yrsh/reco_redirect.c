#include "../include/sh.h"
#include "../include/string.h"

/*
 *=======================================================================================
 *ShRecoRedirect関数
 *コマンドにリダイレクトが含まれているか判定する関数
 *引数
 *char *line
 *シェルに入力された一行
 *=======================================================================================
 */
u8_t ShRecoRedirect(char *line){
	char *word = (char *)memory_alloc(memman, 128);

	/*
	 * > が登場するまでループ
	 */
	while(string_getNext(line, word)){
		if(strcmp(word, ">")){
			puts("Redirect!!");
			return 1;
		}
		line += strlen(word) + 1;
	}

	return 0;
}
