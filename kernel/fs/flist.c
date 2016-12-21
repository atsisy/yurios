#include "../../include/yrfs.h"
#include "../../include/sh.h"

/*
 *=======================================================================================
 *file_list関数
 *カレントディレクトリのファイルを標準出力する関数
 *=======================================================================================
 */
void file_list(char *option) {
	u32_t i;
	struct i_node inode;

	for(i = 0;i < __FILE_OBJECT_ZONE__;i++){
		if(blocks_info[i].exist){
			iread(&inode, i);
			puts(inode.file_name);
		}
	}
}
