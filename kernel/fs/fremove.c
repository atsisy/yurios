#include "../../include/kernel.h"
#include "../../include/yrfs.h"
#include "../../include/sh.h"
#include "../../include/ata.h"
#include "../../include/sysc.h"

/*
 *=======================================================================================
 *RemoveFile関数
 *ファイルを削除する関数
 *引数
 *char *file_name
 *削除するファイル名
 *=======================================================================================
 */
u8_t RemoveFile(char *file_name){
	struct i_node *inode = (struct i_node *)memory_alloc(memman, sizeof(struct i_node));

	i32_t fd = do_open(file_name, __O_RDONLY__);
	iread(inode, fd);

	char zero[512] = { 0 };

	do_write(fd, zero, inode->size);
	write_ata_sector(&ATA_DEVICE0, fd, zero, 1);
	blocks_info[fd].exist = __UNUSED_BLOCK__;

	memory_free(memman, (u32_t)inode, sizeof(struct i_node));

	return 0;
}
