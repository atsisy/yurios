#include "../../include/yrfs.h"
#include "../../include/sysc.h"
#include "../../include/sh.h"

/*
 *=======================================================================================
 *yrfsCreateFs関数
 *新しくyrfsでディスクをフォーマットする関数
 *=======================================================================================
 */
void yrfsCreateFs(){
	//一回全部初期化
	filesystem_zeroclear();

	//ルートディレクトリ作成
	command_mkdir("/");
}
