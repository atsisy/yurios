#include "../../include/kernel.h"
#include "../../include/yrfs.h"
#include "../../include/ata.h"

/*
 *=======================================================================================
 *yrws_write関数
 *補助記憶装置に書き込むそこそこ高レイヤーな関数
 *=======================================================================================
 */
u32_t yrfs_write(u32_t fd, char *buffer, u32_t bytes){

	/*
	 *作業用のinodeを確保
	 */
	struct i_node *inode = (struct i_node *)memory_alloc(memman, sizeof(struct i_node));

	/*
	 *書き込むファイルのinodeを読み込む
	 */
	iread(inode, fd);

	/*
	 *書き込む
	 */
	write_ata_sector(&ATA_DEVICE0, inode->begin_address.sector + inode->seek_address.sector , buffer, byte2sectors(bytes));

	/*
	 *inodeを更新
	 */
	inode->size += (inode->seek_address.offset + bytes);
	iupdate(inode);

	/*
	 *使用済みマークをつける
	 */
	mark_used_sign(inode->begin_address.sector, inode->begin_address.sector+byte2sectors(inode->size));

	/*
	 *メモリを開放
	 */
	memory_free(memman, (u32_t)inode, sizeof(struct i_node));

	return byte2sectors(bytes);
}
