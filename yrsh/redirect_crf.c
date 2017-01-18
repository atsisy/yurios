#include "../include/kernel.h"
#include "../include/sysc.h"
#include "../include/yrfs.h"
#include "../include/string.h"

/*
 *=======================================================================================
 *RedirectCreateFile関数
 *リダイレクト判定後、実際にファイルを生成する関数
 *=======================================================================================
 */
void RedirectCreateFile(char *FileName){
	int fd;
	/*
	 *リダイレクトしようとしているファイルはすでに存在しているか？
	 */
	if(ffind(FileName) == -1){        //存在していない
		fd = do_open(FileName, __O_CREAT__);
		if(fd == -1)
			return;
		do_write(fd, GetOutputStream(), strlen(GetOutputStream()));
		do_close(fd);
	}else{                            //存在している
		RemoveFile(FileName);
		fd = do_open(FileName, __O_CREAT__);
		if(fd == -1)
			return;
		do_write(fd, GetOutputStream(), strlen(GetOutputStream()));
		do_close(fd);
	}

	return;
}
