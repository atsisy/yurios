#include "../../include/bootpack.h"
#include "../../include/ata.h"
#include "../../include/sh.h"

i32_t ata_select_device_ext(struct ATA_DEVICE *device, u8_t ext_flags);
i32_t prepare_wr(struct ATA_DEVICE *device, u32_t lba, i32_t count);

/*
 *=======================================================================================
 *core_read_data関数
 *ATAデバイスから実際にデータを読み込む関数
 *引数
 *struct ATA_DEVICE *device
 *=>ターゲットデバイス
 *u16_t *buffer
 *読み見込んだデータを格納するバッファへのポインタ
 *i32_t length
 *読み込むデータ数
 *返り値
 *無し
 *=======================================================================================
 */
void core_read_ata(struct ATA_DEVICE *device, u16_t *buffer, i32_t length) {

	i32_t i;

	if(buffer != NULL){
		for(i = 0;i <= __ONCE_RW_LIMIT__;i++)
			*buffer++ = io_in16(__ATA_PORT_DATA(device));
	}else{
		/*
		 *なんもデータを読まない
		 */
		for (i = 0; i < __ONCE_RW_LIMIT__; i++)
			io_in16(__ATA_PORT_DATA(device));
	}
}

/*
 *=======================================================================================
 *din_pio関数
 *PIO data in command プロトコル
 *引数
 *u8_t cmd
 *=>実行するATAコマンド
 *struct ATA_DEVICE *device
 *=>ターゲットデバイス
 *void *buffer
 *=>結果を格納するバッファへのポインタ
 *u8_t check_ready
 *=>デバイスチェックフラグ
 *返り値
 *実行結果
 *0...正常終了
 *=======================================================================================
 */
i32_t din_pio(u8_t command, struct ATA_DEVICE *device, void *buffer, i32_t count, u8_t check_ready) {

	i32_t status, i;

	if (check_ready)
		wait_set_drdy(device);

	io_out8(__ATA_PORT_CMD(device), command);

	do_sleep(__ATA_WAIT400ns__);

	io_in8(__ATA_PORT_ALT_STATUS(device));  // 空読み

	u16_t *p = (u16_t *)buffer;

	for (i = 0; i < count; i++) {
		if (wait_bsy_clear(device) < 0) {
			return -2;
		}

		status = io_in8(__ATA_PORT_ALT_STATUS(device));

		if (status & __ATA_STATUS_ERROR__)
			break;  // コマンド実行エラー

		if ((status & __ATA_STATUS_DRQ__) == 0)
			return -3;  // なぜかデータが用意されていない

		core_read_ata(device, p, __ONCE_RW_LIMIT__);
		p += __ONCE_RW_LIMIT__;
	}

	io_in8(__ATA_PORT_ALT_STATUS(device));  // 空読み

	status = io_in8(__ATA_PORT_STATUS(device));

	if(status & __ATA_STATUS_ERROR__){
		return -1;
	}

	return 0;
}

/*
 *=======================================================================================
 *read_ata_sector関数
 *セクタごとにデータを読み込む関数
 *引数
 *struct ATA_DEVICE *device
 *=>ターゲットデバイス
 *u32_t lba
 *LBA方式でセクタを指定
 *void *buffer
 *読み込んだデータを格納するバッファへのポインタ
 *i32_t count
 *=>読み込むデータ数
 *返り値
 *終了状況
 *=======================================================================================
 */
i32_t read_ata_sector(struct ATA_DEVICE *device, u32_t lba, void *buffer, i32_t count) {

	prepare_wr(device, lba, count);

	return din_pio(__ATA_CMD_READ_SECTORS__, device, buffer, count, TRUE);
}
