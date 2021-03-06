#include "../../include/mm.h"
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
i32_t do_chdir(char *pathname){
	i32_t fd = CurrentDirectory.OwnFD, next_file_fd;
	char *line = (char *)kmalloc(11);
	struct i_node *inode = (struct i_node *)kmalloc(sizeof(struct i_node));

	do_close(fd);

	//読み捨て
	//最初の一行ははカレントディレクトリの絶対パス(だったはず)
	gline(fd, line);

	if(strcmp(pathname, "..")){
		char abs_path[256];
		//親ディレクトリに移動
		zeroclear_8array(line, 11);
		gline(fd, line);
		gline((next_file_fd = osAtoi(line)), abs_path);
		puts(abs_path);
		strcpy(CurrentDirectory.AbsPath, abs_path);
		CurrentDirectory.OwnFD = next_file_fd;
		return 0;
	}

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
					return 0;
				}
			}
		}
	}while(true);

	return -1;
}
