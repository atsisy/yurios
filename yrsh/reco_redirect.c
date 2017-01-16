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

/*
 *=======================================================================================
 *Redirect関数
 *リダイレクトを処理する関数
 *=======================================================================================
 */
void Redirect(char *line){
	/*
	 *上流コマンド、下流コマンド、中継バッファの確保
	 */
	char *parent = (char *)memory_alloc(memman, 128);
	char *child = (char *)memory_alloc(memman, 128);
	char *buffer = (char *)memory_alloc_4k(memman, 1024 * 24);

	memory_free(memman, (u32_t)parent, 128);
	memory_free(memman, (u32_t)child, 128);
	memory_free_4k(memman, (u32_t)buffer, 1024 * 24);
}
