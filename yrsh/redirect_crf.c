#include "../include/kernel.h"
#include "../include/sysc.h"
#include "../include/yrfs.h"
#include "../include/string.h"

void CraWrF_ReDir(char *FileName);

/*
 *=======================================================================================
 *RedirectCreateFile関数
 *リダイレクト判定後、実際にファイルを生成する関数
 *=======================================================================================
 */
void RedirectCreateFile(char *FileName){

	/*
	 *リダイレクトしようとしているファイルはすでに存在しているか？
	 */
	if(ffind(FileName) != -1)        //存在している
		RemoveFile(FileName);

	CraWrF_ReDir(FileName);
	return;
}

/*
 *=======================================================================================
 *CraWrF_ReDir関数
 *リダイレクト処理中に使う、ファイル作成とファイル書き込みを一度に行う関数
 *引数
 *char *FileName
 *ファイル名
 *=======================================================================================
 */
void CraWrF_ReDir(char *FileName){
	int fd;
	fd = do_open(FileName, __O_CREAT__);
	if(fd == -1)
		return;
	do_write(fd, GetOutputStream(), strlen(GetOutputStream()));
	do_close(fd);
}
