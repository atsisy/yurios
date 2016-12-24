#include "../../include/yrfs.h"
#include "../../include/sh.h"

static char *yrfs_file_info(char *info, struct i_node *inode);

/*
 *=======================================================================================
 *file_list関数
 *カレントディレクトリのファイルを標準出力する関数
 *=======================================================================================
 */
void file_list(char *option) {
	u32_t i;
	struct i_node *inode = memory_alloc(memman, sizeof(struct i_node));
	char *str = (char *)memory_alloc(memman, 270);
	
	for(i = 0;i < __INODE_LIMIT__;i++){
		if(blocks_info[i].exist){
			iread(inode, i);
			zeroclear_8array(str, 270);
			puts(yrfs_file_info(str, inode));
		}
	}

	memory_free(memman, (u32_t)str, 270);
	memory_free(memman, (u32_t)inode, sizeof(struct i_node));
}

static char *yrfs_file_info(char *info, struct i_node *inode){
	sprintf(info, "%s size:%d inode:%d", inode->file_name, inode->size, inode->id);
	return info;
}
