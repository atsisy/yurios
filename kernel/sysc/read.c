#include "../../include/types.h"
#include "../../include/sh.h"
#include "../../include/ata.h"
#include "../../include/yrfs.h"

/*
 *=======================================================================================
 *do_read関数
 *指定されたストリームから文字列を読み取る関数
 *簡単に言うとreadシステムコールのOS側の処理
 *引数
 *int fd
 *=>指定するストリーム
 *char *buf
 *=>読み取った文字列を格納する変数
 *int count
 *読み取る合計の文字列長
 *返り値
 *実際に読み込んだ文字列の長さ
 *=======================================================================================
 */
size_t do_read(int fd, char *buf, int count){

	struct i_node inode;

	switch(fd){
	case __stdin__:
		/*
		 *標準入力から読み込む
		 */
		type_prompt(buf+1, count);
		break;

	default:
		/*
		 *ファイルを読み込む
		 */

		/*
		 *inode情報を取得
		 */
		iread(&inode, fd);

		/*
		 *読み込むセクタを取得し読み込む
		 */
		read_ata_sector(&ATA_DEVICE0, inode.begin_address.sector+(inode.seek_address.offset/513), buf, 1+(count / 513));
	}

	/*
	 *読み取った文字列長
	 */
	return 0;
}
