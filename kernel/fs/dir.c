#include "../../include/yrfs.h"
#include "../../include/sysc.h"

/*
 *=======================================================================================
 *do_mkdir関数
 *mkdirシステムコールのカーネル内処理
 *=======================================================================================
 */
int do_mkdir(char *pathname, u32_t flags){

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

			//ディレクトリのフラグを立てる
			inode.flags |= __DIRECTORY_FILE__;

                  /*
			 *inodeを補助記憶装置に書き出し
			 */
			fd = iwrite(&inode);

			//blocks_info[fd].exist = __USED_BLOCK__;
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