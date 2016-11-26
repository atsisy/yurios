/*
 *=======================================================================================
 *ATAデバイスの初期化処理を記述するファイル
 *=======================================================================================
 */
#include "../../include/bootpack.h"
#include "../../include/ata.h"
#include "../../include/types.h"
#include "../../include/sh.h"

struct ATA_DEVICE ATA_DEVICE0 = {
	.base_prt = 0x01F0,
	.dv_irq = 14,
	.dv_select = __ATA_DH_SEL_DEV0__,
	.dv_ctrl_prt = 0x03F6,
	.dv_type = __ATA_TYPE_UNKNOWN__
};

struct ATA_DEVICE ATA_DEVICE1 = {
	.base_prt = 0x01F0,
	.dv_irq = 14,
	.dv_select = __ATA_DH_SEL_DEV1__,
	.dv_ctrl_prt = 0x03F6,
	.dv_type = __ATA_TYPE_UNKNOWN__
};

void software_reset(struct ATA_DEVICE *ata_dv, u8_t enable_i32_t);
i32_t initialize_ata_device_sub(struct ATA_DEVICE *device);
void get_signature(struct ATA_DEVICE *dev);
u32_t issue_identify_device_command(struct ATA_DEVICE *device);
void refer_device(struct ATA_DEVICE *device);
i32_t identity_ata_device(struct ATA_DEVICE *device);
void check_device_trans_mode(struct ATA_DEVICE *device);
i32_t initialize_device_trans_mode(struct ATA_DEVICE *device);
i32_t ata_select_device(struct ATA_DEVICE *device);
i32_t wait_busy_and_request_clear(struct ATA_DEVICE *device);
i32_t ata_select_device_ext(struct ATA_DEVICE *device, u8_t ext_flags);
i32_t din_pio(u8_t cmd, struct ATA_DEVICE *device, void *buffer, i32_t count, u8_t check_ready);
i32_t send_non_data(i8_t command, struct ATA_DEVICE *device, u8_t check_flag);
void analyze_identify_data(struct ATA_DEVICE *device);
i32_t issue_idle_command(struct ATA_DEVICE *device);
i32_t command_set_features(struct ATA_DEVICE *device, u8_t sub_cmd, u8_t mode);
i32_t initialize_device_parameters(struct ATA_DEVICE *device);

/*
 *=======================================================================================
 *INITIALIZE_ATA_DEVICE関数
 *ATAデバイスを初期化するボスの関数
 *引数、返り値なし
 *=======================================================================================
 */
void INITIALIZE_ATA_DEVICE(void) {

	puts("Start device initialization");

	puts("Clearing...");
	software_reset(&ATA_DEVICE0, TRUE);

	puts("Start initializing device0...");
	initialize_ata_device_sub(&ATA_DEVICE0);

	puts("Start initializing device1...");
	initialize_ata_device_sub(&ATA_DEVICE1);

	puts("Setting default device...");
	struct ATA_DEVICE *default_dev = &ATA_DEVICE0;

	if (ATA_DEVICE0.dv_type == __ATA_TYPE_PATA__ || ATA_DEVICE0.dv_type == __ATA_TYPE_PATAPI__) {
		/*
		 *何もしない
		 */
	} else if (ATA_DEVICE1.dv_type == __ATA_TYPE_PATA__ || ATA_DEVICE1.dv_type == __ATA_TYPE_PATAPI__) {
		default_dev = &ATA_DEVICE1;
	}

	refer_device(default_dev);

	puts("All done");
	puts("Successful completion.");
}

/*
 *=======================================================================================
 *initialize_ata_device_sub関数
 *ATAデバイス初期化のためのサブルーチン
 *引数
 *struct ATA_DEVICE *device
 *=>初期化するデバイス
 *=======================================================================================
 */
i32_t initialize_ata_device_sub(struct ATA_DEVICE *device) {

	puts("Getting device information...");
	get_signature(device);
	identity_ata_device(device);

	if (device->dv_type != __ATA_TYPE_PATA__ && device->dv_type != __ATA_TYPE_PATAPI__) {
		return -1;
	}

	puts("Checking transfer mode...");
	check_device_trans_mode(device);
	initialize_device_trans_mode(device);

	return 0;
}

/*
 *=======================================================================================
 *initialize_device_trans_mode関数
 *ATAデバイスの転送モードの初期化を行う関数
 *引数
 *struct ATA_DEVICE *device
 *=>対象のATAデバイス
 *=======================================================================================
 */
i32_t initialize_device_trans_mode(struct ATA_DEVICE *device) {

	if (device->dv_type != __ATA_TYPE_PATA__ && device->dv_type != __ATA_TYPE_PATAPI__)
		return -1;

	if (ata_select_device(device) < 0)
		return -1;

	if (device->dv_type == __ATA_TYPE_PATA__) {
		if (initialize_device_parameters(device) < 0)
			return -1;

		if (issue_idle_command(device) < 0)
			return -1;

	} else if (device->dv_type == __ATA_TYPE_PATAPI__) {
		if (device->information->is_support_pow_manage) {
			if (issue_idle_command(device) < 0)
				return -1;
		}
	}

	if (device->information->is_support_iordy) {
		if (command_set_features(device, __SET_TRANSFER__, 0x08 | (device->trans_mode & 7)) < 0) {
			command_set_features(device, __SET_TRANSFER__, 0);
			device->trans_mode = 0;
		}
	} else {
		command_set_features(device, __SET_TRANSFER__, 0);
		device->trans_mode = 0;
	}

	return 0;
}


void refer_device(struct ATA_DEVICE *device) {

	io_out8(__ATA_PORT_DRIVE_HEAD(device), device->dv_select | __ATA_DH_OBS__);
	do_sleep(__ATA_WAIT400ns__);

}

i32_t ata_select_device_ext(struct ATA_DEVICE *device, u8_t ext_flags) {

	wait_busy_and_request_clear(device);

	io_out8(__ATA_PORT_DRIVE_HEAD(device), device->dv_select | __ATA_DH_OBS__ | ext_flags);
	do_sleep(__ATA_WAIT400ns__);

	return wait_busy_and_request_clear(device);
}

/*
 *=======================================================================================
 *identify_ata_device関数
 *ATAデバイス接続確認を行う関数
 *引数
 *struct ATA_DEVICE *device
 *接続確認を行う対象のATAデバイス
 *=======================================================================================
 */
i32_t identity_ata_device(struct ATA_DEVICE *device) {

	i32_t i;
	
	io_out8(__ATA_PORT_DRIVE_HEAD(device), device->dv_select);

	if (device->dv_type == __ATA_TYPE_UNKNOWN__) {
		device->dv_type = __ATA_TYPE_NON__;
		return -1;
	}

	if (wait_bsy_clear(device) < 0) {
		device->dv_type = __ATA_TYPE_UNKNOWN__;
		return -1;
	}

	for (i = 0; i < __RETRY_MAX__; i++) {
		i32_t ret1 = issue_identify_device_command(device);
		do_sleep(__ATA_WAIT5ms__);
		i32_t ret2 = issue_identify_device_command(device);

		if (ret1 != ret2) {
			do_sleep(__ATA_WAIT5ms__);
			continue;
		}

		if (ret1 < 0) {
			device->dv_type = __ATA_TYPE_NON__;
			return -1;
		}

		analyze_identify_data(device);

		return 0;
	}

	device->dv_type = __ATA_TYPE_UNKNOWN__;
	return -1;
}

/*
 *=======================================================================================
 *issue_identify_device_command関数
 +ATAデバイスに対してIDENTIFY DEVICEコマンドを発行する関数
 *引数
 *struct ATA_DEVICE *device
 *=>発行対象のデバイス
 *=======================================================================================
 */
u32_t issue_identify_device_command(struct ATA_DEVICE *device) {

	__SELECT_DEVICE_MACRO(device);

	u32_t cmd = 0;
	u8_t check_ready = FALSE;

	if (device->dv_type == __ATA_TYPE_PATA__) {

		cmd = __ATA_CMD_IDENTIFY_DEVICE__;

		if (device->dv_type != __ATA_TYPE_UNKNOWN__)
			check_ready = TRUE;

	} else if (device->dv_type == __ATA_TYPE_PATAPI__) {
		cmd = __ATAPI_CMD_IDENTIFY_PACKET_DEVICE__;
	}

	return din_pio(cmd, device, device->identify_data, 1, check_ready);
}

/*
 *=======================================================================================
 *initialize_device_parameters関数
 *ATAデバイスのパラメータを初期化する関数
 *引数
 *struct ATA_DEVICE *device
 *対象のATAデバイス
 *=======================================================================================
 */
i32_t initialize_device_parameters(struct ATA_DEVICE *device) {

	if (ata_select_device_ext(device, device->information->establish_head_num - 1) < 0) {
		return -1;
	}

	io_out8(__ATA_PORT_SECTOR_CNT(device), device->information->establish_sector_num);

	return send_non_data(__ATA_CMD_INITIALIZE_DEVICE_PARAMETERS__, device, FALSE);
}

/*
 *=======================================================================================
 *send_non_data関数
 *なんも読まなくて、ATAデバイスの状態を確認する関数
 *引数
 *i8_t command
 *使用するコマンド
 *struct ATA_DEVICE *device
 *=>対象のデバイス
 *u8_t check_flag
 *=>チェックフラグ
 *=======================================================================================
 */
i32_t send_non_data(i8_t command, struct ATA_DEVICE *device, u8_t check_flag) {

	if (check_flag)
		wait_set_drdy(device);

	io_out8(__ATA_PORT_CMD(device), command);

	do_sleep(__ATA_WAIT400ns__);

	if (wait_bsy_clear(device) < 0)
		return -2;

	/*
	 *何も読まない
	 */
	io_in8(__ATA_PORT_ALT_STATUS(device));
	i32_t status = io_in8(__ATA_PORT_STATUS(device));

	if (status & __ATA_STATUS_ERROR__)
		return -1;

	return 0;
}

/*
 *=======================================================================================
 *issue_idle_command関数
 *IDLEコマンドを発行する関数
 *引数
 *struct ATA_DEVICE *device
 *発行対象のデバイス
 *=======================================================================================
 */
i32_t issue_idle_command(struct ATA_DEVICE *device) {

	__SELECT_DEVICE_MACRO(device);

	i32_t cmd;

	if (device->dv_type == __ATA_TYPE_PATA__) {
		cmd = __ATA_CMD_IDLE__;
	} else if (device->dv_type == __ATA_TYPE_PATAPI__) {
		cmd = __ATAPI_CMD_IDLE__;
	} else {
		return -1;
	}

	return send_non_data(cmd, device, TRUE);
}
