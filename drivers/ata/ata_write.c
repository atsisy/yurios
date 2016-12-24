#include "../../include/kernel.h"
#include "../../include/ata.h"
#include "../../include/sh.h"

i32_t ata_select_device_ext(struct ATA_DEVICE *device, u8_t ext_flags);
i32_t prepare_wr(struct ATA_DEVICE *device, u32_t lba, i32_t count);

/*
 *=======================================================================================
 *core_write_data関数
 *ATAデバイスから実際にデータを書き込む関数
 *引数
 *struct ATA_DEVICE *device
 *=>ターゲットデバイス
 *u16_t *buffer
 *書き込むデータが格納されているバッファへのポインタ
 *i32_t length
 *書き込むデータ数
 *返り値
 *無し
 *=======================================================================================
 */
void core_write_ata(struct ATA_DEVICE *device, u16_t *buffer) {
	u16_t i;
	if(buffer != NULL){  //bufferは利用可能
		for(i = 0;i <= __ONCE_RW_LIMIT__;i++)      //データバッファから一つずつ読み出しなが書き込んで行く
			io_out16(__ATA_PORT_DATA(device), *buffer++); //前にすすめる
	}
}

/*
 *=======================================================================================
 *ata_pio_dataout_cmd関数
 *PIO data out command プロトコル
 *引数
 *u8_t cmd
 *=>実行するATAコマンド
 *struct ATA_DEVICE *device
 *=>ターゲットデバイス
 *void *buffer
 *=>書き込む予定のデータが格納されているバッファへのポインタ
 *u8_t check_ready
 *=>デバイスチェックフラグ
 *返り値
 *実行結果
 *0...正常終了
 *=======================================================================================
 */
i32_t dout_pio(u8_t command, struct ATA_DEVICE *device, void *buffer, i32_t count, u8_t check_ready) {

	i32_t status, i;

	if (check_ready)
		wait_set_drdy(device);

	io_out8(__ATA_PORT_CMD(device), command);

	do_sleep(__ATA_WAIT400ns__);

	io_in8(__ATA_PORT_ALT_STATUS(device));  // 空読み

	for (i = 0; i < count; i++) {
		if(wait_bsy_clear(device) < 0)
			return -2;

		status = io_in8(__ATA_PORT_ALT_STATUS(device));

		if (status & __ATA_STATUS_ERROR__)
			break;  // コマンド実行エラー

		if ((status & __ATA_STATUS_DRQ__) == 0)
			return -3;  // なぜかデータが用意されていない

		core_write_ata(device, buffer);
		buffer += __ONCE_RW_LIMIT__;
	}

	io_in8(__ATA_PORT_ALT_STATUS(device));  // 空読み
    
	status = io_in8(__ATA_PORT_STATUS(device));

	if (status & __ATA_STATUS_ERROR__) {
		return -1;
	}

	return 0;
}

/*
 *=======================================================================================
 *ata_write_sector関数
 *ATAデバイスにセクタごとにデータを書き込む関数
 *引数
 *i32_t device
 *=>デバイス選択
 * *i32_t count
 *=>書き込むセクタ数
 *void *buffer
 *=>書き込むデータのバッファへのポインタ
 *返り値
 *1以上
 *=>コマンド実行前エラー
 *0
 *=>正常終了
 *0未満
 *=>コマンド実行ごエラー
 *=======================================================================================
 */
i32_t write_ata_sector(struct ATA_DEVICE *device, u32_t lba, void *buffer, i32_t count) {

	prepare_wr(device, lba, count);

	return dout_pio(__ATA_CMD_WRITE_SECTORS__, device, buffer, count, TRUE);
}
