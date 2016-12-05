#include "../../include/yrfs.h"

/*
 *=======================================================================================
 *ffind関数
 *ファイルを検索する関数
 *結果
 *あった
 *=>そのファイルのinode idを返す
 *ない
 *=>-1を返す
 *=======================================================================================
 */
i32_t ffind(char *pathname) {
	u32_t i, j;
	_Bool w_flag;
	struct i_node inode;

	/*
	 *ファイルの中身領域に到達するまで走査
	 */
	for(i = 0;i < __FILE_OBJECT_ZONE__;i++){
		//inode情報を読み取る
		iread(&inode, i);
		/*
		 *もしも最初の文字があっていたら
		 */
		if(*inode.file_name == *pathname){
			/*
			 *確かめる
			 */
			for(j = 0, w_flag = 0 ;pathname[j] != '\0';j++){
				if(pathname[j] != inode.file_name[j])  //違った
					w_flag = 1;
			}
			/*
			 *あった
			 */
			if(!w_flag)
				return i;
		}
	}

	/*
	 *だめでしたわ〜
	 */
	return -1;
}
