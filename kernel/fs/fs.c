/*
 *=======================================================================================
 *yuri file systems
 *=======================================================================================
 */
#include "../../include/yrfs.h"
#include "../../include/string.h"

i8_t init_inode(struct i_node *inode, char *file_name){
	i32_t i = 0;

	while(file_name[i] != '\0'){
		inode->file_name[i] = file_name[i];
	}
}
