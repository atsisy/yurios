#include "../../include/kernel.h"
#include "../../include/sysc.h"
#include "../../include/yrfs.h"
#include "../../include/string.h"
#include "../../include/sh.h"

extern struct Directory CurrentDirectory;

/*
 *=======================================================================================
 *do_chdir関数
 *カレントディレクトリを移動する関数
 *chdirシステムコールのカーネル内処理
 *=======================================================================================
 */
void do_chdir(char *pathname){
	i32_t fd = CurrentDirectory.OwnFD, next_file_fd;
	char *line = (char *)memory_alloc(memman, 11);
	struct i_node *inode = (struct i_node *)memory_alloc(memman, sizeof(struct i_node));

	do_close(fd);

	//読み捨て
	//最初の一行ははカレントディレクトリの絶対パス(だったはず)
	gline(fd, line);

	do{
		//バッファを初期化
		zeroclear_8array(line, 11);
		gline(fd, line);
		if(!*line)
			break;
		iread(inode, (next_file_fd = osAtoi(line)));

		if(inode->flags & __DIRECTORY_FILE__){   //ディレクトリか？
			if(*pathname == *inode->file_name){    //最初の1文字目はあっているか？
				if(strcmp(pathname, inode->file_name)){   //完全一致か？
					char abs_path[256];
					gline(next_file_fd, abs_path);
					/*
					 *カレントディレクトリを変更
					 */
					strcpy(CurrentDirectory.AbsPath, abs_path);
					CurrentDirectory.OwnFD = next_file_fd;
				}
			}
		}
	}while(true);
}
