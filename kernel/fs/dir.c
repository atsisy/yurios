#include "../../include/yrfs.h"
#include "../../include/sysc.h"
#include "../../include/mm.h"
#include "../../include/string.h"
#include "../../include/sh.h"

//カレントディレクトリ
struct Directory CurrentDirectory;

static void InitDir(struct i_node inode);
static void write_current_dir_info(i32_t new_dir_fd, i32_t crrdir_fd);

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

			//ディレクトリとひも付け
			DirAddFile(inode.id);

		}
	}else{
		puts("mkdir: cannot create directory : File exists");
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
	char *ndir_name = (char *)memory_alloc(memman, (size = (strlen(CurrentDirectory.AbsPath) * strlen(inode.file_name)) + 3));
	zeroclear_8array(ndir_name, size);

	//絶対パスを生成
	strcat(ndir_name, CurrentDirectory.AbsPath);
	// "/"がなかったら追加
	if(GetStringTail(ndir_name) != '/')
		strcat(ndir_name, "/");

	strcat(ndir_name, inode.file_name);

	//追記
	fadd(inode.id, ndir_name);
	fadd(inode.id, "\n");

	//親ディレクトリ情報も渡す
	write_current_dir_info(inode.id, CurrentDirectory.OwnFD);

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
	puts(CurrentDirectory.AbsPath);
}

/*
 *=======================================================================================
 *DirAddFile関数
 *ディレクトリに新しいファイルの情報を書き込む関数
 *=======================================================================================
 */
u8_t DirAddFile(i32_t inode_id){
	//4294967296 <- 10桁 + 1 (NULL文字)
	char inode_id_str[11];
	zeroclear_8array(inode_id_str, 11);

	//文字列に変換
	sprintf(inode_id_str, "%d", inode_id);

	//追記
	fadd(CurrentDirectory.OwnFD, inode_id_str);
	fadd(CurrentDirectory.OwnFD, "\n");

	return SUCCESS;
}

/*
 *=======================================================================================
 *write_current_dir_info関数
 *カレントディレクトリの情報を子ディレクトリに書き込む関数
 *=======================================================================================
 */
static void write_current_dir_info(i32_t new_dir_fd, i32_t crrdir_fd){
	char inode_id_str[11];
	zeroclear_8array(inode_id_str, 11);

	//文字列に変換
	sprintf(inode_id_str, "%d", crrdir_fd);

	//追記
	fadd(new_dir_fd, inode_id_str);
	fadd(new_dir_fd, "\n");
}
