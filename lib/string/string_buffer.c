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
i8_t StringBuffer_Append(struct StringBuffer *str_buf, const char *str){

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

/*
 *=======================================================================================
 *StringBuffer_Reset関数
 *StringBufferが保持する文字列を再代入する関数
 *引数
 *struct StringBuffer *str_buf
 *対象のStringBuffer構造体へのポインタ
 *const char *new_str
 *新しくセットする文字列
 *=======================================================================================
 */
i8_t StringBuffer_Reset(struct StringBuffer *str_buf, const char *new_str){
	if(IS_FAILURE(StringBuffer_Free(str_buf))){
		return FAILURE;
	}

	if(IS_FAILURE(StringBuffer_Append(str_buf, new_str))){
		return FAILURE;
	}

	return SUCCESS;
}

/*
 *=======================================================================================
 *StringBuffer_Insert関数
 *StringBufferの文字列の指定されたindexに文字列を挿入する関数
 *引数
 *struct StringBuffer *str_buf
 *対象のstringbuffer構造体へのポインタ
 *const char *str
 *挿入する文字列
 *int index
 *挿入するインデックス
 *返り値
 *成功
 *SUccess
 *失敗
 *Failure
 *=======================================================================================
 */
i8_t StringBuffer_Insert(struct StringBuffer *str_buf, const char *str, int index){
      return ystring_insert(&str_buf->buffer, str, index);
}

/*
 *=======================================================================================
 *StringBuffer_ReplaceChar関数
 *StringBufferの文字列内の指定した文字を任意の文字に置換する関数
 *引数
 *struct StringBuffer *str_buf
 *対象のstringbuffer構造体へのポインタ
 *char target
 *新しい文字に置換されるべき文字
 *char alternative
 *置換する文字
 *返り値
 *成功
 *SUccess
 *失敗
 *Failure
 *=======================================================================================
 */
i8_t StringBuffer_ReplaceChar(struct StringBuffer *str_buf, char target, char alternative){
      return ystring_replace_char(str_buf->buffer, target, alternative);
}

/*
 *=======================================================================================
 *StringBuffer_ShrinkLoopChar関数
 *StringBufferの文字列中の連続した任意の文字を一文字に圧縮する関数
 *引数
 *struct StringBuffer *str_buf
 *対象のstringbuffer構造体へのポインタ
 *char target_char
 *指定する任意の文字
 *成功
 *SUccess
 *失敗
 *Failure
 *=======================================================================================
 */
i8_t StringBuffer_ShrinkLoopChar(struct StringBuffer *str_buf, char target_char){
      return y_shrink_loop_char(&str_buf->buffer, target_char);
}

/*
 *=======================================================================================
 *StringBuffer_SplitToCharPP関数
 *StringBuffer構造体の文字列を任意の文字で文字列配列に分割する関数
 *引数
 *struct StringBuffer *str_buf
 *対象のstringbuffer構造体へのポインタ
 *char key_word
 *分割に使う文字
 *返り値
 *成功
 *文字列配列へのポインタ
 *失敗
 *NULL
 *=======================================================================================
 */
char **StringBuffer_SplitToCharPP(struct StringBuffer *str_buf, char key_word){
      return split_string(str_buf->buffer, key_word);
}
