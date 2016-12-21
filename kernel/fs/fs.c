/*
 *=======================================================================================
 *yuri file systems
 *=======================================================================================
 */
#include "../../include/yrfs.h"
#include "../../include/sh.h"
#include "../../include/kernel.h"

struct block_info *blocks_info;

/*
 *=======================================================================================
 *init_yrfs関数
 *yuriファイルシステムの初期化する関数
 *=======================================================================================
 */
void init_yrfs() {
	u32_t i, param_y;
	struct i_node inode;

	puts("Start Initializing yurifs...");

	blocks_info = (struct blocks_info *)memory_alloc_4k(memman, __BLOCKS_LIMIT__);

	puts("alloc blocks info structure...");

	param_y = indent << 4;
	
	for(i = 0;i < __INODE_LIMIT__;i++){
		iread(&inode, i);
		/*
		 *ファイル名の先頭がヌル文字のとき空と定義する
		 */
		if(!inode.file_name[0]){   //NULL文字
			blocks_info[i].exist = __UNUSED_BLOCK__;
			boxfill8(binfo->vram, binfo->scrnx, 0, 0, param_y, 64, param_y+16);
			
			print_value(i, 0, param_y);

		}else{
			u32_t n;
			blocks_info[i].exist = __USED_BLOCK__;
			for(n = inode.begin_address.sector;n < inode.end_address.sector;n++){
				blocks_info[n].exist = __USED_BLOCK__;
			}
		}

	}
	indent_shell();
	puts("Complete Initializing yurifs.");
}
