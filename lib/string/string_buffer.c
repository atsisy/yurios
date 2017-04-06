#include "../../include/kernel.h"
#include "../../include/string.h"

/*
 *=======================================================================================
 *create_string_buffer関数
 *StringBufferを確保する関数
 *引数
 *なし
 *=======================================================================================
 */
struct StringBuffer *create_string_buffer(const char *init_str){
	struct StringBuffer *new_buffer = (struct StringBuffer *)memory_alloc(memman, sizeof(struct StringBuffer));
	new_buffer->write_point = 0;
	StringBuffer_Append(new_buffer, init_str);
	return new_buffer;
}

/*
 *=======================================================================================
 *StringBuffer_Append関数
 *StringBufferに文字列を追加する関数
 *引数
 *struct StringBuffer *str_buf
 *対象のStringBuffer構造体へのポインタ
 *const char *str
 *追加する文字列
 *返り値
 *成功
 *SUccess
 *失敗
 *Failure
 *MEMO
 *strcpyでコピーするのでstrは破壊されない
 *=======================================================================================
 */
u8_t StringBuffer_Append(struct StringBuffer *str_buf, const char *str){

	if((str_buf->buffer = (char *)memory_alloc(memman, strlen(str) + 1)))
		return FAILURE;

	strcpy(str_buf->buffer, str);
	str_buf->write_point++;

	return SUCCESS;
}

/*
 *=======================================================================================
 *StringBuffer_Length関数
 *StringBufferに格納された文字列を返す関数
 *=======================================================================================
 */
size_t StringBuffer_Length(struct StringBuffer *str_buf){
	if(!str_buf->buffer)
		return 0;
	return strlen(str_buf->buffer);
}

/*
 *=======================================================================================
 *StringBuffer_CharAt関数
 *StringBufferに格納された文字列にインデックスで文字にアクセスし、それを返す関数
 *=======================================================================================
 */
char StringBuffer_CharAt(struct StringBuffer *str_buf, u32_t index){
	if(!str_buf->buffer)
		return 0;
	return str_buf->buffer[index];
}
