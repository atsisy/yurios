#include "../../include/yrfs.h"
#include "../../include/ata.h"
#include "../../include/sh.h"

//inodeを書き込み可能状態にする関数
static void writable_inode(struct i_node *inode, struct writable_data *data);
//writable_dataからinode情報を解釈する関数
static void translate_wrdata2inode(struct i_node *inode, struct writable_data *data);

/*
 *=======================================================================================
 *icreat関数
 *inodeを作る関数
 *引数
 *char *
 *ファイル名
 *=======================================================================================
 */
struct i_node icreat(char *file_name) {
	struct i_node inode;
      u32_t i = 0;

	/*
	 *まずファイル名をコピー
	 */
	while(*file_name != '\0'){
		inode.file_name[i] = *file_name++;
		i++;
	}

	i = 0;
	//inode領域の空きを探す
	while(blocks_info[i].exist == __USED_BLOCK__) i++;
	inode.id = i;
	blocks_info[i].exist = __USED_BLOCK__;

	/*
	 *次にファイル割り当て
	 */
	i = __FILE_OBJECT_ZONE__;
	while(blocks_info[i].exist == __USED_BLOCK__) i++;

	blocks_info[i].exist = __USED_BLOCK__;

	/*
	 *今の所、隙間に割り当てるようなスキルは無いのでからのセクタにそのまま
	 */
	inode.begin_address.offset = 0;
	inode.begin_address.sector = i;

	/*
	*最初はbegin_addressと同じ
	*/
	inode.seek_address.offset = 0;
	inode.seek_address.sector = i;

	/*
	 *ファイルにはまだ何も書き込まれていないはずなので開始も終了も同じ
	 */
	inode.end_address.offset = 0;
	inode.end_address.sector = i;

	/*
	 *サイズ0
	 */
	inode.size = 0;

	return inode;
}

/*
 *=======================================================================================
 *iwrite関数
 *inode情報を補助記憶装置に書き込む
 *=======================================================================================
 */
u32_t iwrite(struct i_node *inode) {

	struct writable_data *data = NULL;

	/*
	 *inodeを書き込み可能形式にする
	 */
	new_wrdata(data, __WRITABLE_INODE_SIZE__);
	writable_inode(inode, data);

      /*
	 *書き込む
	 */
	write_ata_sector(&ATA_DEVICE0, inode->id, data->data, __WRITABLE_INODE_SECTORS__);

	/*
	 *ちゃんと開放
	 */
	delete_wrdata(data);

	return inode->id;
}

/*
 *=======================================================================================
 *iread関数
 *inode情報を補助記憶装置から読み込む関数
 *=======================================================================================
 */
void iread(struct i_node *inode, u32_t index) {

	/*
	 *writable_data構造体の準備
	 */
	struct writable_data *data = NULL;
	new_wrdata(data, __WRITABLE_INODE_SIZE__);

	zeroclear_8array(inode->file_name, 256);

	/*
	 *読み込む
	 */
	read_ata_sector(&ATA_DEVICE0, index, data->data, __WRITABLE_INODE_SECTORS__);

	/*
	 *inodeに変換
	 */
	translate_wrdata2inode(inode, data);

	/*
	 *ちゃんと開放
	 */
	delete_wrdata(data);
}

/*
 *=======================================================================================
 *inodeを書き込み可能状態にする関数
 *=======================================================================================
 */
static void writable_inode(struct i_node *inode, struct writable_data *data) {
	u32_t i, *data_p = data->data;
	char *unp = inode->cr_user;
	char *fnp = inode->file_name;

	/*
	 *ID、アドレス、サイズ、作成日時をコピー
	 */

	//inode ID
	*data_p = inode->id;
	data_p++;

	//開始アドレス
	*data_p = inode->begin_address.sector;
	data_p++;
	*data_p = inode->begin_address.offset;
	data_p++;

	//シークアドレス
	*data_p = inode->seek_address.sector;
	data_p++;
	*data_p = inode->seek_address.offset;
	data_p++;

	//終了アドレス
	*data_p = inode->end_address.sector;
	data_p++;
	*data_p = inode->end_address.offset;
	data_p++;

	//サイズ
	*data_p = inode->size;
	data_p++;

	//パーミッション
	*data_p = inode->permission;
	data_p++;

	//作成日時
	*data_p = inode->cr_time;
	data_p++;

	//フラグ
	*data_p = inode->flags;
	data_p++;

	//ファイルの作成者名
	for(i = 0;i < 16; i++, data_p++, unp+=4)
		char4tou32(unp, data_p);

	//ファイル名
	for(i = 0;i < 64; i++, data_p++, fnp+=4)
	      char4tou32(fnp, data_p);

}

/*
 *=======================================================================================
 *translate_wrdata2inode関数
 *writable_dataからinode情報を解釈する関数
 *=======================================================================================
 */
static void translate_wrdata2inode(struct i_node *inode, struct writable_data *data) {
	u32_t i;

	//ファイル名へのポインタを先に入れておく
	char *fnp = inode->file_name;
	char *unp = inode->cr_user;

	/*
	 *アドレスとサイズをコピー
	 */

	//inode ID
	inode->id = data->data[0];

	//開始アドレス
	inode->begin_address.sector = data->data[1];
	inode->begin_address.offset = data->data[2];

	//シークアドレス
	inode->seek_address.sector = data->data[3];
	inode->seek_address.offset = data->data[4];

	//終了アドレス
	inode->end_address.sector = data->data[5];
	inode->end_address.offset = data->data[6];

	//サイズ
	inode->size = data->data[7];

	//パーミッション
	inode->permission = data->data[8];

	//作成日時
	inode->cr_time = data->data[9];

	//フラグ
	inode->flags = data->data[10];

	//このファイルの作成者名
	for(i = 0;i < 16; i++, unp+=4)
		u32to4char(data->data[i+11], unp);

	//ファイル名
	for(i = 0;i < 64; i++, fnp+=4)
		u32to4char(data->data[i+27], fnp);

}
