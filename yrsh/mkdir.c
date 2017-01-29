#include "../include/yrfs.h"
#include "../include/sysc.h"

/*
 *=======================================================================================
 *command_mkdir関数
 *mkdirコマンドの内部処理
 *=======================================================================================
 */
void command_mkdir(char *pathname){
	char *dir_name = pathname + 6;
	do_close(do_mkdir(dir_name, __O_CREAT__));
}
