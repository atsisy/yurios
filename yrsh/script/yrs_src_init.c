#include "../../include/yrfs.h"
#include "../../include/kernel.h"
#include "../../include/sh.h"
#include "../../include/sysc.h"

struct YRS_SRC *yrs_src_init(char *file_name){
      struct YRS_SRC *src_file = (struct YRS_SRC *)memory_alloc(memman, sizeof(struct YRS_SRC));
      src_file->fd = do_open(file_name, __O_RDONLY__);
      
      if(src_file->fd != -1){
            load_yrs(src_file);
      }

      return src_file;
}
