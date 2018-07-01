#include <mm.h>
#include "../include/sh.h"
#include "../include/string.h"

/*
 *ストリーム構造体たち
 */
struct Stream *InputStream;
struct Stream *OutputStream;
struct Stream *ErrorStream;

/*
 *=======================================================================================
 *InitStreams関数
 *入出力ストリームを初期化する関数
 *=======================================================================================
 */
void InitStreams(){

	InputStream = (struct Stream *)memory_alloc(memman, sizeof(struct Stream));
	OutputStream = (struct Stream *)memory_alloc(memman, sizeof(struct Stream));
	ErrorStream = (struct Stream *)memory_alloc(memman, sizeof(struct Stream));

	/*
	 *メモリを確保
	 *確保するメモリ量はvalue.hに記述
	 */
	InputStream->buffer = (char *)memory_alloc_4k(memman, __INPUT_STREAM_SIZE__);
	OutputStream->buffer = (char *)memory_alloc_4k(memman, __OUTPUT_STREAM_SIZE__);
	ErrorStream->buffer = (char *)memory_alloc_4k(memman, __ERROR_STREAM_SIZE__);

      /*
	 *バッファを0クリア
	 */
	zeroclear_8array(InputStream->buffer, __INPUT_STREAM_SIZE__);
	zeroclear_8array(OutputStream->buffer, __OUTPUT_STREAM_SIZE__);
	zeroclear_8array(ErrorStream->buffer, __ERROR_STREAM_SIZE__);

	/*
	 *書き込み位置をリセット
	 */
	OutputStream->write_point = InputStream->write_point = ErrorStream->write_point = 0;

	puts("Complete initializing Streams.");
}

/*
 *=======================================================================================
 *GetOutputStream関数
 *出力ストリームのバッファを返す関数
 *=======================================================================================
 */
char *GetOutputStream(){
	return OutputStream->buffer;
}

/*
 *=======================================================================================
 *GetInputStream関数
 *入力ストリームのバッファを返す関数
 *=======================================================================================
 */
char *GetInputStream(){
	return InputStream->buffer;
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
	if(OutputStream->write_point > __OUTPUT_STREAM_SIZE__ - 0xff)
		OutputStream->write_point = 0;

	/*
	 *コピー
	 */
	strcpy(OutputStream->buffer + OutputStream->write_point, str);

	/*
	 *書き込み位置をすすめる
	 */
	OutputStream->write_point += (strlen(str) + 1);

}

/*
 *=======================================================================================
 *WriteErrorStream関数
 *エラーストリームに書き込む関数
 *=======================================================================================
 */
void WriteErrorStream(char *str){

	/*
	 *オーバーフローしそうなら元に戻す
	 */
	if(ErrorStream->write_point > __ERROR_STREAM_SIZE__ - 0xff)
		ErrorStream->write_point = 0;

	/*
	 *コピー
	 */
	strcpy(ErrorStream->buffer + ErrorStream->write_point, str);

	/*
	 *書き込み位置をすすめる
	 */
	ErrorStream->write_point += (strlen(str) + 1);

}

/*
 *=======================================================================================
 *ResetOutputWritePoint関数
 *出力ストリームの書き込み位置を0にリセットする関数
 *=======================================================================================
 */
static void ResetOutputWritePoint(){
	OutputStream->write_point = 0;
}

/*
 *=======================================================================================
 *ResetInputWritePoint関数
 *入力ストリームの書き込み位置を0にリセットする関数
 *=======================================================================================
 */
static void ResetInputWritePoint(){
	InputStream->write_point = 0;
}

/*
 *=======================================================================================
 *ResetErrorWritePoint関数
 *エラーストリームの書き込み位置を0にリセットする関数
 *=======================================================================================
 */
static void ResetErrorWritePoint(){
	ErrorStream->write_point = 0;
}

/*
 *=======================================================================================
 *ResetOutputStream関数
 *出力ストリームを初期化する関数
 *=======================================================================================
 */
void ResetOutputStream(){
	ResetOutputWritePoint();
	zeroclear_8array(OutputStream->buffer, __OUTPUT_STREAM_SIZE__);
}

/*
 *=======================================================================================
 *ResetInputStream関数
 *出力ストリームを初期化する関数
 *=======================================================================================
 */
void ResetInputStream(){
	ResetInputWritePoint();
	zeroclear_8array(InputStream->buffer, __INPUT_STREAM_SIZE__);
}

/*
 *=======================================================================================
 *ResetErrorStream関数
 *エラーストリームを初期化する関数
 *=======================================================================================
 */
void ResetErrorStream(){
	ResetErrorWritePoint();
	zeroclear_8array(ErrorStream->buffer, __ERROR_STREAM_SIZE__);
}
