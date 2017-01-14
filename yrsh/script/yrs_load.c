#include "../../include/yrfs.h"
#include "../../include/sh.h"
#include "../../include/sysc.h"

void load_yrs(struct YRS_SRC *yrs_src){
      //inode構造体を宣言
      struct i_node inode;
      //inode読み込み
      iread(&inode, yrs_src->fd);

      //得たサイズからソースバッファを確保
      yrs_src->source = (char *)memory_alloc(memman, inode.size);

      //ソース読み込み
      do_read(yrs_src->fd, yrs_src->source, inode.size);
      
      //終了
      return;
}
