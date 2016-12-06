/*
 *=======================================================================================
 *yuri file systems
 *=======================================================================================
 */
#include "../../include/yrfs.h"

struct block_info blocks_info[100];

void init_yrfs() {
	u32_t i = 0;
	struct i_node inode;
	
	for(;i < 100;i++){
		iread(&inode, i);
		/*
		 *ファイル名の先頭がヌル文字のとき空と定義する
		 */
		blocks_info[i].empty = inode.file_name[0] == '\0';
	}
}
