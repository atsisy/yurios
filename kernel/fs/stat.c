#include "../../include/yrfs.h"
#include "../../include/sh.h"

/*
 *=======================================================================================
 *do_stat関数
 *statシステムコールの中身
 *=======================================================================================
 */
u32_t do_stat(int fd, u32_t *box) {
	u32_t i;
	char *fnp;
	struct i_node inode;

	/*
	 *inode情報を取得
	 */
	iread(&inode, fd);

	/*
	 *ファイル内容へのポインタをコピー
	 */
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

