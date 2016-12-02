/*
 *=======================================================================================
 *yuri file systems
 *=======================================================================================
 */
#include "../../include/yrfs.h"
#include "../../include/string.h"

struct block_info blocks_info[100];

void init_yrfs() {
	u32_t i = 0;
	for(;i < 100;i++){
		blocks_info[i].empty = 1;
	}
	inode_id = 1;
}
