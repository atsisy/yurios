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
	char *fnp, *unp;
	struct i_node inode;

	/*
	 *inode情報を取得
	 */
	iread(&inode, fd);

      /*
	 *ID、アドレス、サイズ、作成日時をコピー
	 */

	//inode ID
	*box = inode.id;
	box++;
	//開始アドレス
	*box = inode.begin_address.sector;
	box++;
	*box = inode.begin_address.offset;
	box++;
	//終了アドレス
	*box = inode.end_address.sector;
	box++;
	*box = inode.end_address.offset;
	box++;
	//サイズ
	*box = inode.size;
	box++;
	//パーミッション
	*box = inode.permission;
	box++;
	//作成日時
	*box = inode.cr_time;
	box++;

	//ファイルの作成者名
	for(i = 0, unp = inode.cr_user;i < 16; i++, box++, unp+=4)
		char4tou32(unp, box);
	
	//ファイル名
	for(i = 0, fnp = inode.file_name;i < 64; i++, box++, fnp+=4)
	      char4tou32(fnp, box);
	
	return 0;
}

