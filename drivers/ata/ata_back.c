#include "../../include/mm.h"
#include "../../include/ata.h"
#include "../../include/sh.h"
#include "../../include/sysc.h"

i32_t send_non_data(i8_t command, struct ATA_DEVICE *device, u8_t check_flag);
void refer_device(struct ATA_DEVICE *device);

/*
 *=======================================================================================
 *wait_set_drdy関数
 *DRDYビットがセットされるまで待つ関数
 *引数
 *struct ATA_DEVICE *device
 *=>このATAデバイスのビットを見る
 *=======================================================================================
 */
i32_t wait_set_drdy(struct ATA_DEVICE *device){

	i32_t i;

	for (i = 0;i < __ATA_TIMEOUT__;i++) {
		if (io_in8(__ATA_PORT_ALT_STATUS(device)) & __ATA_STATUS_DRDY__)
			return 0;
	}

	/*
	 *タイムアウト
	 */
	return -1;
}

/*
 *=======================================================================================
 *analyze_identify_data関数
 *デバイスの認識情報を検証していく関数
 *引数
 *=>検証したいデバイス
 *=======================================================================================
 */
void analyze_identify_data(struct ATA_DEVICE *device) {

	u16_t *identify = device->identify_data;
	struct ATA_DEVICE_INFORMATION *info = (struct ATA_DEVICE_INFORMATION *)kmalloc(sizeof(struct ATA_DEVICE_INFORMATION));

	device->information = info;

	info->establish_cylinder_num = identify[1];
	info->establish_head_num     = identify[3];
	info->establish_sector_num   = identify[6];

	info->is_support_iordy = (i8_t)(identify[49] & 0x0800);

	if (identify[53] & 2) {
		info->support_pio_mode = identify[64] & 0x007f;
	} else {
		info->support_pio_mode = 0;
	}

	info->current_cylinder_num = identify[54];
	info->current_head_num     = identify[55];
	info->current_sector_num   = identify[56];

	/*
	 *LBA方式が使用可能かの判定
	 */
	if ((identify[60] | identify[61]) != 0) {
		/*
		 *可能
		 */
		info->is_support_lba = true;
	} else {
		/*
		 *無理
		 */
		info->is_support_lba = false;
	}

	info->is_support_pow_manage = identify[82] & 8;

}

/*
 *=======================================================================================
 *check_device_trans_mode関数
 *デバイスの転送モードをチェックし指定する関数
 *引数
 *struct ATA_DEVICE *device
 *=>目的のデバイス
 *=======================================================================================
 */
void check_device_trans_mode(struct ATA_DEVICE *device) {

	if (device->dv_type != __ATA_TYPE_PATA__ && device->dv_type != __ATA_TYPE_PATAPI__) {
		device->trans_mode = PIO_MODE_NON;
	}

	if (device->information->support_pio_mode & __SUPPORT_PIO_MODE4__) {
		device->trans_mode = PIO_MODE4;
	} else if (device->information->support_pio_mode & __SUPPORT_PIO_MODE3__) {
		device->trans_mode = PIO_MODE3;
	} else {
		device->trans_mode = PIO_MODE0;
	}
}

/* BSYビットがクリアされるまでウェイト */
i32_t wait_bsy_clear(struct ATA_DEVICE *device) {

	u32_t i;

	for (i = 0; i < __ATA_TIMEOUT__; i++) {
		/*
		 *普通に戻ってきた
		 */
		if ((io_in8(__ATA_PORT_ALT_STATUS(device)) & __ATA_STATUS_BSY__) == 0)
			return 0;
	}

	/*
	 *タイムアウトした
	 */
	return -1;
}

/*
 *=======================================================================================
 *wait_busy_and_request_clear関数
 *BUSYビットとDRQビットがクリアされるまで待つ関数
 *=======================================================================================
 */
i32_t wait_busy_and_request_clear(struct ATA_DEVICE *device) {

	i32_t i;

	for (i = 0; i < __ATA_TIMEOUT__; i++) {
		i32_t status = io_in8(__ATA_PORT_ALT_STATUS(device));
		if ((status & (__ATA_STATUS_BSY__ | __ATA_STATUS_DRQ__)) == 0)
			return 0;
	}

	/*
	 *タイムアウト
	 */
	return -1;
}

/*
 *=======================================================================================
 *software_reset関数
 *ソフトウェアリセットを行う関数
 *
 *=======================================================================================
 */
void software_reset(struct ATA_DEVICE *ata_dv, u8_t enable_i32_t) {
	u8_t flag;

	if(enable_i32_t)
		flag = 0;
	else
		flag = __ATA_DC_nIEN__;

	io_out8(ata_dv->dv_ctrl_prt, __ATA_DC_SRST__ | flag); 	      //ソフトウェアリセット実行
	do_sleep(__ATA_WAIT400ns__);
	io_out8(ata_dv->dv_ctrl_prt, flag);  				//ソフトウェアリセット終了
	do_sleep(__ATA_WAIT400ns__);
	// ソフトウェア・リセット直後はデバイス0が選択されている
}

/*
 *=======================================================================================
 *command_set_features関数
 *=======================================================================================
 */
i32_t command_set_features(struct ATA_DEVICE *device, u8_t sub_cmd, u8_t mode) {

    __SELECT_DEVICE_MACRO(device);

    io_out8(__ATA_PORT_FEATURES(device), sub_cmd);
    io_out8(__ATA_PORT_SECTOR_CNT(device), mode);

    return send_non_data(__ATA_CMD_SET_FEATURES__, device, true);
}

/*
 *=======================================================================================
 *ata_select_device関数
 *=======================================================================================
 */
i32_t ata_select_device(struct ATA_DEVICE *device) {
    wait_busy_and_request_clear(device);

    io_out8(__ATA_PORT_DRIVE_HEAD(device), device->dv_select | __ATA_DH_OBS__);
    do_sleep(__ATA_WAIT400ns__);

    return wait_busy_and_request_clear(device);
}

/*
 *=======================================================================================
 *get_signature関数
 *=======================================================================================
 */
void get_signature(struct ATA_DEVICE *device) {

	i32_t cylinder_low, cylinder_high, signal, sel_dev, i;

	device->dv_type = __ATA_TYPE_UNKNOWN__;

	for (i = 0; i < __RETRY_MAX__; i++) {
		refer_device(device);

		/*
		 *未接続
		 */
		if (io_in8(__ATA_PORT_STATUS(device)) == 0xFF)
			break;

		/*
		 *未接続
		 */
		if (wait_bsy_clear(device) < 0)
			break;

		/*
		 *デバイスがだめ
		 */
		if ((io_in8(__ATA_PORT_ERR(device)) & 0x7F) != 1)
			break;

		sel_dev = io_in8(__ATA_PORT_DRIVE_HEAD(device)) & 0x10;

		if (sel_dev == device->dv_select) {
			cylinder_low = io_in8(__ATA_PORT_CYL_LO(device));
			cylinder_high = io_in8(__ATA_PORT_CYL_HI(device));
			signal = (cylinder_high << 8) | cylinder_low;

			switch (signal) {
			case __ATA_SIGNATURE_PATA__:
				device->dv_type = __ATA_TYPE_PATA__;
				break;

			case __ATA_SIGNATURE_PATAPI__:
				device->dv_type = __ATA_TYPE_PATAPI__;
				break;

			default:
				device->dv_type = __ATA_TYPE_UNKNOWN__;
			}

			break;
		}
	}
}
