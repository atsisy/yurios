#include "../../include/yrfs.h"
#include "../../include/sh.h"

void file_list(char *option) {
	u32_t i;
	struct i_node inode;
	
	for(i = 0;i < __FILE_OBJECT_ZONE__;i++){
		if(!blocks_info[i].empty){
			iread(&inode, i);
			puts(inode.file_name);
		}
	}
}
