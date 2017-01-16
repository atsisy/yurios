#include "../include/sh.h"
#include "../include/string.h"

/*
 *=======================================================================================
 *ShRecoRedirect関数
 *コマンドにリダイレクトが含まれているか判定する関数
 *=======================================================================================
 */
u8_t ShRecoRedirect(char *line){
	char *word = (char *)memory_alloc(memman, 128);

	string_getNext(line, word);
	return 0;
}
