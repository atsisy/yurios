#include "../../include/kernel.h"
#include "../../include/string.h"
#include "../../include/util_macro.h"

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

	if(IS_FAILURE((int)(str_buf->buffer = (char *)memory_alloc(memman, strlen(str) + 1))))
		return FAILURE;

	zeroclear_8array(str_buf->buffer, strlen(str));
	strcpy(str_buf->buffer, str);

	return SUCCESS;
}

/*
 *=======================================================================================
 *StringBuffer_Length関数
 *StringBufferに格納された文字列を返す関数
 *=======================================================================================
 */
size_t StringBuffer_Length(struct StringBuffer *str_buf){
	if(IS_NULLPO(str_buf->buffer))
		return 0;
	return strlen(str_buf->buffer);
}

/*
 *=======================================================================================
 *StringBuffer_CharAt関数
 *StringBufferに格納された文字列にインデックスで文字にアクセスし、それを返す関数
 *返り値
 *成功
 *文字
 *失敗
 *Failure
 *=======================================================================================
 */
char StringBuffer_CharAt(struct StringBuffer *str_buf, u32_t index){
	if(IS_NULLPO(str_buf->buffer))
		return FAILURE;
	return str_buf->buffer[index];
}

/*
 *=======================================================================================
 *StringBuffer_Clean関数
 *StringBufferのバッファをフリーする関数
 *引数
 *struct StringBuffer *str_buf
 *フリーするStringBuffer
 *返り値
 *成功
 *SUccess
 *失敗
 *Failure
 *=======================================================================================
 */
i8_t StringBuffer_Clean(struct StringBuffer *str_buf){
	if(IS_NULLPO(str_buf->buffer))
		return FAILURE;

	memory_free(memman, (u32_t)(str_buf->buffer), strlen(str_buf->buffer));
	return SUCCESS;
}

/*
 *=======================================================================================
 *StringBuffer_Free関数
 *StringBufferを破棄する関数
 *引数
 *struct StringBuffer *str_buf
 *破棄するStringBuffer
 *返り値
 *成功
 *SUccess
 *失敗
 *Failure
 *=======================================================================================
 */
i8_t StringBuffer_Free(struct StringBuffer *str_buf){
	if(IS_NULLPO(str_buf))
		return FAILURE;

	if(IS_NULLPO(str_buf->buffer)){
		if(IS_FAILURE(StringBuffer_Clean(str_buf))){
			return FAILURE;
		}
	}

	if(IS_FAILURE(memory_free(memman, (u32_t)str_buf, sizeof(struct StringBuffer)))){
		return FAILURE;
	}

	return SUCCESS;
}
