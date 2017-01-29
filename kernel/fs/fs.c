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

extern char *CURRENT_DIRECTORY_NAME;

/*
 *=======================================================================================
 *init_yrfs関数
 *yuriファイルシステムの初期化する関数
 *=======================================================================================
 */
void init_yrfs() {
	u32_t i, param_y, n;
	struct i_node inode;

	puts("Start Initializing yurifs...");

	blocks_info = (struct block_info *)memory_alloc_4k(memman, __BLOCKS_LIMIT__);

	puts("alloc blocks info structure...");

	param_y = indent << 4;

	//カレントディレクトリ名を初期化
	CURRENT_DIRECTORY_NAME = (char *)memory_alloc(memman, __CURRENT_DIR_STR_SIZE__);
	zeroclear_8array(CURRENT_DIRECTORY_NAME, __CURRENT_DIR_STR_SIZE__);

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
			n = inode.begin_address.sector;

			blocks_info[i].exist = __USED_BLOCK__;
			for(;n < byte2sectors(inode.size);n++){
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
		if(blocks_info[i].exist == __UNUSED_BLOCK__){   //NULL文字
			if(i % 100 == 0){
				boxfill8(binfo->vram, binfo->scrnx, 0, 0, param_y, 16, param_y+16);
				print_value(i/100, 0, param_y);
			}

		}else{
			/*
			 *有効なinode書き込みを検知
			 */
			u32_t n = inode.begin_address.sector, j;

			/*
			 *まずinodeを削除
			 */
			write_ata_sector(&ATA_DEVICE0, i, zero, 1);
			blocks_info[i].exist = __UNUSED_BLOCK__;

			/*
			 *ファイル本体を削除
			 */
			for(j = 0;j < byte2sectors(inode.size);j++, n++){
				write_ata_sector(&ATA_DEVICE0, n, zero, 1);
				blocks_info[n].exist = __UNUSED_BLOCK__;
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
	puts("done");
}

/*
 *=======================================================================================
 *mark_used_sign関数
 *ある位置からある位置まで、使用中のマークつける関数
 *返り値
 *最後にマークしたイデックス
 *=======================================================================================
 */
u32_t mark_used_sign(u32_t start, u32_t end){
	/*
	 *startからendまで
	 */
	while(start <= end){
		/*
		 *使用中のマーク
		 */
		blocks_info[start].exist = __USED_BLOCK__;
		start++;
	}

	return start;
}
