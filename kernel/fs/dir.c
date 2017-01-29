#include "../../include/yrfs.h"
#include "../../include/sysc.h"
#include "../../include/kernel.h"
#include "../../include/string.h"
#include "../../include/sh.h"

//カレントディレクトリの文字列
char *CURRENT_DIRECTORY_NAME;

static void InitDir(struct i_node inode);

/*
 *=======================================================================================
 *do_mkdir関数
 *mkdirシステムコールのカーネル内処理
 *=======================================================================================
 */
i32_t do_mkdir(char *pathname, u32_t flags){

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

			//最初の書き込み
			InitDir(inode);
		}
	}
	if(flags & __O_RDONLY__){
		/*
		 *ffind関数をつかってinode idを求めて返す
		 */
		return ffind(pathname);
	}

	do_close(fd);

	return fd;
}

/*
 *=======================================================================================
 *InitDir関数
 *ディレクトリファイルの最初の書き込みを行う関数
 *=======================================================================================
 */
static void InitDir(struct i_node inode){
	//ディレクトリの絶対パスの文字列長
	int size;

	//絶対パス用のバッファを確保
	char *ndir_name = (char *)memory_alloc(memman, (size = (strlen(CURRENT_DIRECTORY_NAME) * strlen(inode.file_name)) + 3));
	//絶対バスを生成
	strcat(ndir_name, CURRENT_DIRECTORY_NAME);
	strcat(ndir_name, inode.file_name);
	strcat(ndir_name, "/");

	//追記
	fadd(inode.id, ndir_name);

	//メモリ解放
	memory_free(memman, (u32_t)ndir_name, size);
}

/*
 *=======================================================================================
 *command_pwd関数
 *pwdコマンドに内部処理
 *=======================================================================================
 */
void command_pwd(){
	//標準出力
	puts(CURRENT_DIRECTORY_NAME);
}
