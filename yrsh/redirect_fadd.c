#include "../include/kernel.h"
#include "../include/sysc.h"
#include "../include/yrfs.h"
#include "../include/string.h"

void CraWrF_ReDir(char *FileName);

/*
 *=======================================================================================
 *RedirectFileAddSub関数
 *ファイル追記形式リダイレクトの判定後、実際にファイルに追記する関数
 *=======================================================================================
 */
void RedirectFileAddSub(char *FileName){

	/*
	 *リダイレクトしようとしているファイルはすでに存在しているか？
	 */
	if(ffind(FileName) != -1){        //存在している
		int fd = do_open(FileName, __O_RDONLY__);
		fadd(fd, GetOutputStream());
	}else{                            //存在していないので通常のリダイレクト処理
		CraWrF_ReDir(FileName);
	}

	return;
}
