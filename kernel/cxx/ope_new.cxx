#include "../../include/yurixx.hxx"

/*
 *=======================================================================================
 *new演算子を定義する
 *=======================================================================================
 */

//new
void *operator new(u32_t size){
	void *memory = (void *)memory_alloc(memman, size);
	return memory;
}

//new[]
void *operator new[](u32_t size){
	void *memory = (void *)memory_alloc(memman, size);
	return memory;
}


/*
 *=======================================================================================
 *グローバル変数等で使用する配置new演算子
 *=======================================================================================
 */

//配置new
void *operator new(u32_t size, void *memory){
	return memory;
}

//配置new[]
void *operator new[](u32_t size, void *memory){
	return memory;
}
