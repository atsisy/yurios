#include "../../include/yrfs.h"
#include "../../include/sh.h"

/*
 *=======================================================================================
 *do_seek関数
 *seekシステムコールを実行する関数
 *引数
 *int fd
 *=>ファイルディスクリプタ
 *off_t offset
 *オフセット
 *int whence
 *シーク方法
 *=======================================================================================
 */
off_t do_seek(int fd, off_t offset, int whence) {
	struct i_node inode;

	/*
	 *inode情報を読み取り
	 */
	iread(&inode, fd);

	switch(whence){
	case __SEEK_SET__:
		/*
		 *ファイルの始まりからシーク
		 */
		inode.begin_address.offset = offset;
		break;
	case __SEEK_CUR__:
		/*
		 *現在のオフセットからシーク
		 */
		inode.begin_address.offset += offset;
		break;
	case __SEEK_END__:
		/*
		 *ファイルの終わりからシーク
		 */
		inode.begin_address.offset = inode.size + offset;
		break;
	}

	/*
	 *新しいinode情報を書き込み
	 */
	iwrite(&inode);

	return offset;
}
