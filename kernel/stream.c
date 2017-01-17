#include "../include/kernel.h"
#include "../include/sh.h"
#include "../include/string.h"

/*
 *ストリームのバッファ
 */
char *InputStream;
char *OutputStream;

/*
 *ストリームの書き込み位置
 */
int input_write_point;
int output_write_point;

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

	/*
	 *書き込み位置をリセット
	 */
	output_write_point = input_write_point = 0;

	puts("Complete initializing Streams.");
}

/*
 *=======================================================================================
 *GetOutputStream関数
 *出力ストリームのバッファを返す関数
 *=======================================================================================
 */
char *GetOutputStream(){
	return OutputStream;
}

/*
 *=======================================================================================
 *GetInputStream関数
 *入力ストリームのバッファを返す関数
 *=======================================================================================
 */
char *GetInputStream(){
	return InputStream;
}

/*
 *=======================================================================================
 *WriteOutputStream関数
 *出力ストリームに書き込む関数
 *=======================================================================================
 */
void WriteOutputStream(char *str){

	/*
	 *オーバーフローしそうなら元に戻す
	 */
	if(output_write_point > __OUTPUT_STREAM_SIZE__ - 0xff)
		output_write_point = 0;

	/*
	 *コピー
	 */
	strcpy(OutputStream + output_write_point, str);

	/*
	 *書き込み位置をすすめる
	 */
	output_write_point += (strlen(str) + 1);

}

/*
 *=======================================================================================
 *ResetOutputWritePoint関数
 *出力ストリームの書き込み位置を0にリセットする関数
 *=======================================================================================
 */
static void ResetOutputWritePoint(){
	output_write_point = 0;
}

/*
 *=======================================================================================
 *ResetInputWritePoint関数
 *入力ストリームの書き込み位置を0にリセットする関数
 *=======================================================================================
 */
static void ResetInputWritePoint(){
	input_write_point = 0;
}

/*
 *=======================================================================================
 *ResetOutputStream関数
 *出力ストリームを初期化する関数
 *=======================================================================================
 */
void ResetOutputStream(){
	ResetOutputWritePoint();
	zeroclear_8array(OutputStream, __OUTPUT_STREAM_SIZE__);
}

/*
 *=======================================================================================
 *ResetInputStream関数
 *出力ストリームを初期化する関数
 *=======================================================================================
 */
void ResetInputStream(){
	ResetInputWritePoint();
	zeroclear_8array(InputStream, __INPUT_STREAM_SIZE__);
}
