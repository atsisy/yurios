#include "../include/kernel.h"
#include "../include/sh.h"

/*
 *ストリームのバッファ
 */
char *InputStream;
char *OutputStream;

/*
 *=======================================================================================
 *InitStreams関数
 *入出力ストリームを初期化する関数
 *=======================================================================================
 */
void InitStreams(){
	puts("Initialize Streams.");
	/*
	 *メモリを確保
	 *確保するメモリ量はvalue.hに記述
	 */
	InputStream = (char *)memory_alloc_4k(memman, __INPUT_STREAM_SIZE__);
	InputStream = (char *)memory_alloc_4k(memman, __OUTPUT_STREAM_SIZE__);
	puts("Complete initializing Streams.");
}
