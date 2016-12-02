#include "../../include/yrfs.h"

int do_open(char *pathname, u32_t flags) {

	int fd = 0;
	if(flags & __O_CREAT__){
		struct i_node inode = icreat(pathname);
		
		iwrite(&inode);
		fd = inode.address.sector;
	}
	return fd;
}
