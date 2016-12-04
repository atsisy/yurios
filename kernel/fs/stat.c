#include "../../include/yrfs.h"

/*
 *=======================================================================================
 *do_stat関数
 *statシステムコールの中身
 *=======================================================================================
 */
i32_t do_stat(int fd, u32_t *box) {
	u32_t i;
	char *fnp;
	struct i_node inode;

	iread(&inode, fd);

	fnp = inode.file_name;

	*box = inode.id;
	box++;
	*box = inode.address.sector;
	box++;
	*box = inode.address.offset;
	box++;
	*box = inode.size;

	for(i = 0;i < 64; i++, fnp+=4, box++)
		char4tou32(fnp, box);

	return 0;
}

