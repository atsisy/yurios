#include "../../include/yrfs.h"
#include "../../include/sh.h"
#include "../../include/sysc.h"
#include "../../include/string.h"
#include "../../include/mm.h"

static char *yrfs_file_info(char *info, struct i_node *inode);
extern struct Directory CurrentDirectory;

/*
 *=======================================================================================
 *file_list関数
 *カレントディレクトリのファイルを標準出力する関数
 *=======================================================================================
 */
void file_list(char *option) {
	u32_t i;
	struct i_node *inode = (struct i_node *)memory_alloc(memman, sizeof(struct i_node));
	char *str = (char *)memory_alloc(memman, 270);
	
	for(i = 0;i < __INODE_LIMIT__;i++){
		if(blocks_info[i].exist == __USED_BLOCK__){
			iread(inode, i);
			zeroclear_8array(str, 270);
			puts(yrfs_file_info(str, inode));
		}
	}

	memory_free(memman, (u32_t)str, 270);
	memory_free(memman, (u32_t)inode, sizeof(struct i_node));
}


/*
 *=======================================================================================
 *getline関数
 *改行で区切ってファイルの中身を返す
 *=======================================================================================
 */
void gline(int fd, char *line) {

	u32_t  i, p;
	u32_t *box = (u32_t *)memory_alloc(memman, sizeof(u32_t) * 128);
	char *buffer = (char *)memory_alloc(memman, 1024);

	p = 0;

	do_stat(fd, box);
	i = box[4];

	while(1){
		do_read(fd, buffer, 512);
		for(;i < 512; i++, p++ ){
			switch(buffer[i]){
				//改行
			case 0x0a:
			case '\0':
				line[p] = '\0';
				do_seek(fd, i+1, __SEEK_SET__);

				memory_free(memman, (u32_t)box, sizeof(u32_t) * 128);
				memory_free(memman, (u32_t)buffer, 1024);
				return;
			default:
				line[p] = buffer[i];
			}
		}
	}

}


void nFileList(char *option) {
        i32_t fd = CurrentDirectory.OwnFD;
	char *line = (char *)memory_alloc(memman, 11);
	struct i_node *inode = (struct i_node *)memory_alloc(memman, sizeof(struct i_node));

	do_close(fd);

	//読み捨て
	gline(fd, line);

	do{
		zeroclear_8array(line, 11);
		gline(fd, line);
		if(!*line)
			break;
		iread(inode, osAtoi(line));
		puts(inode->file_name);
	}while(true);
}



/*
 *=======================================================================================
 *yrfs_file_info関数
 *inode情報を表示可能な形式で返す関数
 *=======================================================================================
 */
static char *yrfs_file_info(char *info, struct i_node *inode){
	sprintf(info, "%s size:%d inode:%d", inode->file_name, inode->size, inode->id);
	return info;
}
