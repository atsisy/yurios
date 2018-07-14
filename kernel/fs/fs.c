/*
 *=======================================================================================
 *yuri file systems
 *=======================================================================================
 */
#include "../../include/yrfs.h"
#include "../../include/sh.h"
#include "../../include/mm.h"
#include "../../include/ata.h"
#include "../../include/sysc.h"

struct block_info *blocks_info;

extern struct Directory CurrentDirectory;

u8_t CreateRootDir(void);
void MountRootDir(void);

/*
 *=======================================================================================
 *init_yrfs関数
 *yuriファイルシステムの初期化する関数
 *=======================================================================================
 */
void init_yrfs() {
	u32_t i, n;
	struct i_node inode;

	puts("Start Initializing yurifs...");

	blocks_info = (struct block_info *)kmalloc(__BLOCKS_LIMIT__);

	puts("alloc blocks info structure...");

	for(i = 0;i < __INODE_LIMIT__;i++){
		iread(&inode, i);
		/*
		 *ファイル名の先頭がヌル文字のとき空と定義する
		 */
		if(!inode.file_name[0]){   //NULL文字
			blocks_info[i].exist = __UNUSED_BLOCK__;
			if(i % 100 == 0){
                                printk("%d\n", i / 100);
			}

		}else{
			n = inode.begin_address.sector;

			blocks_info[i].exist = __USED_BLOCK__;
			for(;n < byte2sectors(inode.size);n++){
				blocks_info[n].exist = __USED_BLOCK__;
			}
			if(i % 100 == 0){
                                printk("%d\n", i / 100);
			}
		}

	}

        printk("%d\n", 100);

	//ルートディレクトリがなければ作成
	CreateRootDir();
	MountRootDir();
	
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
	u32_t i;
	struct i_node inode;
				
	for(i = 0;i < __INODE_LIMIT__;i++){
		iread(&inode, i);
		/*
		 *ファイル名の先頭がヌル文字のとき空と定義する
		 */
		if(blocks_info[i].exist == __UNUSED_BLOCK__){   //NULL文字
			if(i % 100 == 0){
                                printk("%d\n", i / 100);
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
                                printk("%d\n", i / 100);
			}
		}

	}
        
	printk("%d\n", 100);
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
        i32_t tmp;
