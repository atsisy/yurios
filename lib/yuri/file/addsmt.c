#include "../../../include/sh.h"
#include "../../../include/yrfs.h"


/*
 *=======================================================================================
 *fadd関数
 *ファイルの末尾に追記する関数
 *=======================================================================================
 */
void fadd(int fd, char *buffer) {
	char read_buf[512];
	struct i_node inode;
	iread(&inode, fd);

      do_seek(fd, 0, __SEEK_END__);
	do_read(fd, read_buf, 1);
	
}
