#include "../../include/kernel.h"
#include "../../include/yrfs.h"
#include "../../include/sh.h"
#include "../../include/ata.h"
#include "../../include/sysc.h"
#include "../../include/string.h"

static struct i_node *file_exist_current_dir(char *file_name);

/*
 *=======================================================================================
 *RemoveFile関数
 *ファイルを削除する関数
 *引数
 *char *file_name
 *削除するファイル名
 *=======================================================================================
 */
u8_t RemoveFile(char *file_name){

	struct i_node *inode = file_exist_current_dir(file_name);

	if(!inode){
		print("rm: cannot remove ");
		print(file_name);
		puts(": No such file or directory");
		return FAILURE;
	}

	char zero[512] = { 0 };

	do_write(inode->id, zero, inode->size);
	write_ata_sector(&ATA_DEVICE0, inode->id, zero, 1);
	blocks_info[inode->id].exist = __UNUSED_BLOCK__;

	memory_free(memman, (u32_t)inode, sizeof(struct i_node));

	return 0;
}

/*
 *=======================================================================================
 *file_exist_current_dir関数
 *カレントディレクトリに指定されたファイルが存在するか判定する関数
 *引数
 *探すファイル名
 *返り値
 *inode構造体へのポインタ
 *存在しない場合、NULLが帰る
 *=======================================================================================
 */
static struct i_node *file_exist_current_dir(char *file_name){
	extern struct Directory CurrentDirectory;

	i32_t fd = CurrentDirectory.OwnFD;
	char *line = (char *)memory_alloc(memman, 11);
	struct i_node *inode = (struct i_node *)memory_alloc(memman, sizeof(struct i_node));

	do_close(fd);

	//読み捨て
	gline(fd, line);
	zeroclear_8array(line, 11);

	while(gline(fd, line) == SUCCESS){
		if(!*line)
			break;
		iread(inode, osAtoi(line));
		if(strcmp(inode->file_name, file_name)){
			return inode;
		}
		zeroclear_8array(line, 11);
	}

	return NULL;
}
