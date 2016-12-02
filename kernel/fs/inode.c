#include "../../include/yrfs.h"
#include "../../include/ata.h"

//inodeを書き込み可能状態にする関数
static void writable_inode(struct i_node *inode, struct writable_data *data);
//writable_dataからinode情報を解釈する関数
static void translate_wrdata2inode(struct i_node *inode, struct writable_data *data);

//inode ID付与の際に用いる変数
u32_t inode_id;
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

	/*
	 *次にファイル割り当て
	 */
	i = __FILE_OBJECT_ZONE__;
	while(blocks_info[i].empty)
		i++;
	/*
	 *今の所、隙間に割り当てるようなスキルは無いのでからのセクタにそのまま
	 */
	inode.address.offset = 0;
	inode.address.sector = i;

	inode.id = inode_id;
	inode_id++;

	return inode;
}

/*
 *=======================================================================================
 *iwrite関数
 *inode情報を補助記憶装置に書き込む
 *=======================================================================================
 */
void iwrite(struct i_node *inode) {
	u32_t i = 0;
	struct writable_data *data = NULL;

	/*
	 *inodeを書き込み可能形式にする
	 */
	new_wrdata(data, __WRITABLE_INODE_SIZE__);
	writable_inode(inode, data);

	//空きを探す
	while(!blocks_info[i].empty) i++;

      /*
	 *書き込む
	 */
	write_ata_sector(&ATA_DEVICE0, i, data->data, __WRITABLE_INODE_SIZE__);

	//使用済みフラグ
	blocks_info[i].empty = 0;

	/*
	 *ちゃんと開放
	 */
	delete_wrdata(data);
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

	/*
	 *読み込む
	 */
	read_ata_sector(&ATA_DEVICE0, index, data->data, __WRITABLE_INODE_SIZE__);

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
	u32_t i;

	/*
	 *ID、アドレス、サイズをコピー
	 */
	data->data[0] = inode->id;
	data->data[1] = inode->address.sector;
	data->data[2] = inode->address.offset;
	data->data[3] = inode->size;

	/*
	 *ファイル名を圧縮して代入
	 */
	for(i = 0;i < 64;i++){
		data->data[i+4] =
			(u32_t)inode->file_name[i<<2]           |
			((u32_t)inode->file_name[(i<<2)+1]) <<  8 |
			((u32_t)inode->file_name[(i<<2)+2]) << 16 |
			((u32_t)inode->file_name[(i<<2)+3]) << 24;
	}
}

/*
 *=======================================================================================
 *translate_wrdata2inode関数
 *writable_dataからinode情報を解釈する関数
 *=======================================================================================
 */
static void translate_wrdata2inode(struct i_node *inode, struct writable_data *data) {
	u32_t i;

	/*
	 *アドレスとサイズをコピー
	 */
	inode->id = data->data[0];
	inode->address.sector = data->data[1];
	inode->address.offset = data->data[2];
	inode->size = data->data[3];

	/*
	 *圧縮を展開
	 */
	for(i = 0;i < 64;i++){
		inode->file_name[(i<<2)]   = (char)data->data[i+4];
		inode->file_name[(i<<2)+1] = (char)(data->data[i+4] >>  8);
		inode->file_name[(i<<2)+2] = (char)(data->data[i+4] >> 16);
		inode->file_name[(i<<2)+3] = (char)(data->data[i+4] >> 24);
	}
}
