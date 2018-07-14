#include "../../include/yrfs.h"
#include "../../include/sysc.h"
#include "../../include/mm.h"
#include "../../include/string.h"
#include "../../include/sh.h"
#include "../../include/yrfs.h"

extern struct Directory CurrentDirectory;

/*
 *=======================================================================================
 *CreateRootDir関数
 *ルートディレクトリが存在しない場合、作成する関数
 *=======================================================================================
 */
u8_t CreateRootDir(void){

	int fd = 0;
	struct i_node inode;

	/*
	 *同名のファイルが存在しない
	 */
	if(ffind("/") == -1){
		/*
		 *inode作成
		 */
		inode = icreat("/");

		//ディレクトリのフラグを立てる
		inode.flags |= __DIRECTORY_FILE__;

		/*
		 *inodeを補助記憶装置に書き出し
		 */
		fd = iwrite(&inode);

		//最初の書き込み

		fadd(inode.id, "/\n");

		do_close(fd);

		return SUCCESS;
	}

	return FAILURE;
}

/*
 *=======================================================================================
 *MountRootDir関数
 *ルートディレクトリをカレントディレクトリにする関数
 *=======================================================================================
 */
void MountRootDir(void){
	//カレントディレクトリ名を初期化
	CurrentDirectory.AbsPath = (char *)kmalloc(__CURRENT_DIR_STR_SIZE__);
	zeroclear_8array(CurrentDirectory.AbsPath, __CURRENT_DIR_STR_SIZE__);

	//初期値をルートディレクトリからロード
	*CurrentDirectory.AbsPath = '/';
	CurrentDirectory.OwnFD = ffind("/");
}
