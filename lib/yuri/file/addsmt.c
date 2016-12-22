#include "../../../include/sh.h"
#include "../../../include/yrfs.h"


/*
 *=======================================================================================
 *fadd関数
 *ファイルの末尾に追記する関数
 *=======================================================================================
 */
void fadd(int fd, char *buffer) {
	char rw_buf[512];
	u32_t i, arg_index;
	struct i_node inode;

	iread(&inode, fd);

	do_seek(fd, 0, __SEEK_END__);
	do_read(fd, rw_buf, 1);

	for(i = inode.begin_address.offset % 512, arg_index = 0;i < 512;i++, arg_index++){
		if(!buffer[arg_index])
			break;
		rw_buf[i] = buffer[arg_index];
	}

	do_write(fd, rw_buf, 20);

}
