#include "../../include/kernel.h"
#include "../../include/string.h"
#include "../../include/util_macro.h"

/*
 *=======================================================================================
 *create_string_vector関数
 *StringVector構造体を確保する関数
 *引数なし
 *返り値
 *成功
 *確保したStringVector構造体へのポインタ
 *失敗
 *NULL
 *=======================================================================================
 */
struct StringVector *create_string_vector(){
      struct StringVector *str_vec =  (struct StringVector *)memory_alloc(memman, sizeof(struct StringVector));

      if(IS_NULLPO(str_vec))
         return NULL;

      str_vec->size = 0;
      str_vec->buffers = NULL;

      return str_vec;
}


