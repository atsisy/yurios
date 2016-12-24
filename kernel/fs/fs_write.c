#include "../../include/kernel.h"
#include "../../include/yrfs.h"
#include "../../include/ata.h"

/*
 *=======================================================================================
 *yrws_write関数
 *補助記憶装置に書き込むそこそこ高レイヤーな関数
 *=======================================================================================
 */
void yrfs_write(u32_t fd, char *buffer, u32_t length){
	struct i_node *inode = memory_alloc(memman, sizeof(struct i_node));
	iread(inode, fd);
	
	write_ata_sector(&ATA_DEVICE0, inode->seek_address.sector, buffer, );

	memory_free(memman, (u32_t)inode, sizeof(struct i_node));
}
