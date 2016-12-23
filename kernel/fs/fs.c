/*
 *=======================================================================================
 *yuri file systems
 *=======================================================================================
 */
#include "../../include/yrfs.h"
#include "../../include/sh.h"
#include "../../include/kernel.h"
#include "../../include/ata.h"

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
			if(i % 100 == 0){
				boxfill8(binfo->vram, binfo->scrnx, 0, 0, param_y, 16, param_y+16);
				print_value(i/100, 0, param_y);
			}

		}else{
			u32_t n = inode.begin_address.sector;

			blocks_info[i].exist = __USED_BLOCK__;
			for(;n < inode.end_address.sector;n++){
				blocks_info[n].exist = __USED_BLOCK__;
			}
			if(i % 100 == 0){
				boxfill8(binfo->vram, binfo->scrnx, 0, 0, param_y, 16, param_y+16);
				print_value(i/100, 0, param_y);
			}
		}

	}

	boxfill8(binfo->vram, binfo->scrnx, 0, 0, param_y, 64, param_y+16);
	print_value(100, 0, param_y);
	
	indent_shell();
	puts("Complete Initializing yurifs.");
}

/*
 *=======================================================================================
 *filesystem_zeroclear関数
 *ファイルシステムをゼロクリアする関数
 *=======================================================================================
 */
void filesystem_zeroclear(){
	u8_t zero[512] = { 0 };
	u32_t i, param_y = indent << 4;
	struct i_node inode;
				
	for(i = 0;i < __INODE_LIMIT__;i++){
		iread(&inode, i);
		/*
		 *ファイル名の先頭がヌル文字のとき空と定義する
		 */
		if(!inode.file_name[0]){   //NULL文字
			blocks_info[i].exist = __UNUSED_BLOCK__;
			if(i % 100 == 0){
				boxfill8(binfo->vram, binfo->scrnx, 0, 0, param_y, 16, param_y+16);
				print_value(i/100, 0, param_y);
			}

		}else{
			u32_t n = inode.begin_address.sector;
			write_ata_sector(&ATA_DEVICE0, i, zero, 1);
			write_ata_sector(&ATA_DEVICE0, n, zero, 1);
			blocks_info[i].exist = __UNUSED_BLOCK__;
			blocks_info[n].exist = __UNUSED_BLOCK__;
		
			if(i % 100 == 0){
				boxfill8(binfo->vram, binfo->scrnx, 0, 0, param_y, 16, param_y+16);
				print_value(i/100, 0, param_y);
			}
		}

	}
	boxfill8(binfo->vram, binfo->scrnx, 0, 0, param_y, 64, param_y+16);
	print_value(100, 0, param_y);
	indent_shell();
	puts("done");
}