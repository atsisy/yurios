#include "../../include/yrfs.h"
#include "../../include/bootpack.h"

/*
 *=======================================================================================
 *new_wrdata関数
 *新しくwritable_data構造体を確保する関数
 *=======================================================================================
 */
void new_wrdata(struct writable_data *data, u32_t size) {
	data->size = size;
	data->data = (u32_t *)memory_alloc(memman, size * sizeof(u32_t));
}


/*
 *=======================================================================================
 *delete_wrdata関数
 *writable_data構造体を開放する関数
 *=======================================================================================
 */
void delete_wrdata(struct writable_data *data) {
	memory_free(memman, (u32_t)data->data, data->size * sizeof(u32_t));
}
