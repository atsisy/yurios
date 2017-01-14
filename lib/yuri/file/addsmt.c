#include "../../../include/sh.h"
#include "../../../include/yrfs.h"
#include "../../../include/string.h"
#include "../../../include/sysc.h"


/*
 *=======================================================================================
 *fadd関数
 *ファイルの末尾に追記する関数
 *=======================================================================================
 */
void fadd(int fd, char *buffer) {
	char rw_buf[512];
	u32_t i = 0, arg_index;
	struct i_node inode;

	iread(&inode, fd);
	do_read(fd, rw_buf, 1);

	/*
	do_seek(fd, 0, __SEEK_END__);

	for(i = inode.seek_address.offset % 512, arg_index = 0;i < 512;i++, arg_index++){
		if(!buffer[arg_index])
			break;
		rw_buf[i] = buffer[arg_index];
	}
	*/
	while(rw_buf[i]){
		/*
		*do_read(fd, rw_buf, 1);
		*/
		i++;
	}
	
	for(arg_index = 0;i < 512;i++, arg_index++){
		if(!buffer[arg_index])
			break;
		rw_buf[i] = buffer[arg_index];
	}


	do_write(fd, rw_buf, strlen(buffer));

}
