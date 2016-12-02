#include "../../include/yrfs.h"
#include "../../include/ata.h"

//inodeを書き込み可能状態にする関数
static void writable_inode(struct i_node *inode, struct writable_data *data);

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
		inode.file_name[i] = *file_name;
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

	return inode;
}

/*
 *=======================================================================================
 *iwrite関数
 *inode情報を補助記憶装置に書き込む
 *=======================================================================================
 */
void iwrite(struct i_node *inode) {
	u32_t i= 0;
	struct writable_data *data = NULL;

	/*
	 *inodeを書き込み可能形式にする
	 */
	new_wrdata(data, __WRITABLE_INODE_SIZE__);
	writable_inode(inode, data);

	//空きを探す
	while(blocks_info[i].empty);

      /*
	 *書き込む
	 */
	write_ata_sector(&ATA_DEVICE0, i, data->data, __WRITABLE_INODE_SIZE__);

	/*
	 *ちゃんと開放
	 */
	delete_wrdata(data);
}

void iread(struct i_node *inode) {
	
}
/*
 *=======================================================================================
 *inodeを書き込み可能状態にする関数
 *=======================================================================================
 */
static void writable_inode(struct i_node *inode, struct writable_data *data) {
	u32_t i;

	/*
	 *アドレスとサイズをコピー
	 */
	data->data[0] = inode->address.sector;
	data->data[1] = inode->address.offset;
	data->data[2] = inode->size;

	/*
	 *ファイル名を圧縮して代入
	 */
	for(i = 0;i < 64;i++){
		data->data[i+3] =
			((u32_t)inode->file_name[i])   << 24 |
			((u32_t)inode->file_name[i+1]) << 16 |
			((u32_t)inode->file_name[i+2]) <<  8 |
			(u32_t)inode->file_name[i+3];
	}
}