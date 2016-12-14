#include "../../include/types.h"
#include "../../include/sh.h"
#include "../../include/ata.h"
#include "../../include/yrfs.h"

/*
 *=======================================================================================
 *do_write関数
 *指定されたストリームに文字列を書き込む関数
 *簡単に言うとwriteシステムコールのOS側の処理
 *引数
 *int fd
 *=>指定するストリーム
 *char *buf
 *=>書き込みたい文字列
 *int count
 *=>書き込む長さ
 *返り値
 *実際に書き込んだ文字列の長さ
 *=======================================================================================
 */
size_t do_write(int fd, char *buf, int count){
	/*
	 *サイズを保持しておく変数
	 */
	int size = 0;
	struct i_node inode;

	switch(fd){
		/*
		 *標準出力に書き込む
		 */
	case __stdout__:
		while(*buf != 0 && count > size){
			/*
			 *標準出力に書き込む
			 */
			put_char(*buf);
			buf++;
			size++;
		}
		break;
	default:
		/*
		 *ファイルに書き込む
		 */

		/*
		 *inode情報を取得
		 */
		iread(&inode, fd);
		/*
		 *書き込むセクタを取得し書き込む
		 */
		write_ata_sector(&ATA_DEVICE0, inode.seek_address.sector, buf, 1+(count / 513));
	}

	return size;
}
