#include "../../include/yrfs.h"

int do_open(char *pathname, u32_t flags){
	if(flags & __O_CREAT__) {
		struct i_node inode = icreat(pathname);
		iwrite(&inode);
	}
	return 0;
}
