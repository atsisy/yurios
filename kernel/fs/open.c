#include "../../include/yrfs.h"
#include "../../include/sh.h"

int do_open(char *pathname, u32_t flags) {

	int fd = 0;
	struct i_node inode;
	
	if(flags & __O_CREAT__){
		/*
		*同名のファイルが存在しない
		*/
		if(ffind(pathname) == -1){
                 /*
		 	*inode作成
		 	*/
			inode = icreat(pathname);
			/*
			 *inodeを補助記憶装置に書き出し
			 */
			fd = iwrite(&inode);
			blocks_info[fd].empty = __USED_BLOCK__;
		}
	}
	if(flags & __O_RDONLY__){
		/*
		 *ffind関数をつかってinode idを求めて返す
		 */
		return ffind(pathname);
	}

	return fd;
}
