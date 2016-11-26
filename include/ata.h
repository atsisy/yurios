#ifndef ATA_H
#define ATA_H

#include "../include/types.h"

/*
 *SET FEATURESコマンド用サブコマンド
 */
#define __SET_TRANSFER__	0x03	//Set Transfer Mode

//定数定義
#define __ATA_TIMEOUT__		10000000l	//タイムアウトループ回数
#define __RETRY_MAX__		8			//リトライ回数
#define __ATA_WAIT400ns__	1			//400nsウェイトループ回数
#define __ATA_WAIT5ms__		12500

#define __SUPPORT_PIO_MODE3__  1
#define __SUPPORT_PIO_MODE4__  2

//シグネチャ
#define __ATA_SIGNATURE_PATA__    0x0000
#define __ATA_SIGNATURE_PATAPI__  0xeb14


//デバイス
#define __ATA_TYPE_NON__     (1 << 0)  // 未接続
#define __ATA_TYPE_UNKNOWN__ (1 << 1)  // 不明デバイス
#define __ATA_TYPE_PATA__    (1 << 2)
#define __ATA_TYPE_PATAPI__  (1 << 3)


//モード
#define PIO_MODE0     0
#define PIO_MODE3     3
#define PIO_MODE4     4
#define PIO_MODE_NON  0xff


//ポート
#define __ATA_PORT_DATA(device)          ((device)->base_prt + 0)
#define __ATA_PORT_FEATURES(device)      ((device)->base_prt + 1)
#define __ATA_PORT_ERR(device)           ((device)->base_prt + 1)
#define __ATA_PORT_SECTOR_CNT(device)    ((device)->base_prt + 2)
#define __ATA_PORT_SECTOR_NO(device)     ((device)->base_prt + 3)
#define __ATA_PORT_LBA_LOW(device)       ((device)->base_prt + 3)
#define __ATA_PORT_CYL_LO(device)        ((device)->base_prt + 4)
#define __ATA_PORT_LBA_MID(device)       ((device)->base_prt + 4)
#define __ATA_PORT_CYL_HI(device)        ((device)->base_prt + 5)
#define __ATA_PORT_LBA_HI(device)        ((device)->base_prt + 5)
#define __ATA_PORT_DRIVE_HEAD(device)    ((device)->base_prt + 6)
#define __ATA_PORT_CMD(device)           ((device)->base_prt + 7)
#define __ATA_PORT_STATUS(device)        ((device)->base_prt + 7)
#define __ATA_PORT_DEV_CTRL(device)      ((device)->dv_ctrl_prt)
#define __ATA_PORT_ALT_STATUS(device)    ((device)->dv_ctrl_prt)

//Status レジスタ
#define __ATA_STATUS_ERROR__   (1 << 0)  // エラー
#define __ATA_STATUS_DRQ__     (1 << 3)  // データ・リクエスト。1 = データ転送要求しているとき
#define __ATA_STATUS_DRDY__    (1 << 6)  // デバイス・レディ。1 = デバイスがレディのとき
#define __ATA_STATUS_BSY__     (1 << 7)  // ビジー


//Device/Head レジスタ
#define __ATA_DH_SEL_DEV0__  0         //マスタ
#define __ATA_DH_SEL_DEV1__  (1 << 4)  //スレーブ
#define __ATA_DH_LBA__       (1 << 6)
#define __ATA_DH_OBS__       0xa0

//Device Control レジスタ
#define __ATA_DC_nIEN__  (1 << 1)  // i32_tRQ信号の有効／無効
#define __ATA_DC_SRST__  (1 << 2)  // ソフトウェア・リセット

#define __ATA_CMD_READ_SECTORS__              0x20
#define __ATA_CMD_WRITE_SECTORS__             0x30
#define __ATA_CMD_IDLE__                      0xe3
#define __ATA_CMD_IDENTIFY_DEVICE__           0xec
#define __ATA_CMD_SET_FEATURES__              0xef
#define __ATA_CMD_INITIALIZE_DEVICE_PARAMETERS__  0x91

#define __ATAPI_CMD_IDENTIFY_PACKET_DEVICE__  0xa1
#define __ATAPI_CMD_IDLE__                    0xe1

//====================================================================================================================
//構造体定義
//====================================================================================================================

/* ATAコマンド発行用 構造体 */
struct STRUCT_ATA_CMD {
	u8_t feature;	/* フィ－チャーレジスタ */
	u8_t sec_cnt;	/* セクタカウントレジスタ */
	u8_t sec_no;	/* セクタナンバレジスタ */
	u8_t cyl_lo;	/* シリンダ下位レジスタ */
	u8_t cyl_hi;	/* シリンダ上位レジスタ */
	u8_t dev_hed;	/* デバイス/ヘッドレジスタ */
	u8_t command;	/* コマンドレジスタ */
	u8_t DRDY_Chk;	/* コマンドレジスタ */
};

/*
*ATAデバイスの情報を保持しておく構造体
*/
struct ATA_DEVICE_INFORMATION {
    u16_t establish_cylinder_num;  			// 論理シリンダ数（デフォルト転送モード）
    u16_t establish_head_num;      	// 論理ヘッド数  （デフォルト転送モード）
    u16_t establish_sector_num;    	// 論理セクタ数  （デフォルト転送モード）
    u16_t current_cylinder_num;      // 論理シリンダ数（現在の転送モード）
    u16_t current_head_num;          // 論理ヘッド数  （現在の転送モード）
    u16_t current_sector_num;        // 論理セクタ数  （現在の転送モード）
    u8_t support_pio_mode;
    i8_t is_support_lba;
    i8_t is_support_iordy;
    i8_t is_support_pow_manage;
};

/*
*ATAデバイスの状態を保持しておく構造体
*/
struct ATA_DEVICE {
    i16_t base_prt;
    i16_t dv_irq;
	i16_t trans_mode;				//転送モード
	i16_t dv_select;  				//マスタかスレーブか
    i16_t dv_ctrl_prt;  			//Device Control Registers/Alternate Status ports
    i16_t dv_type;	 				//デバイスの種類
    u16_t identify_data[256];		//IDENTIFY DEVICE情報格納配列(256ワード)
    struct ATA_DEVICE_INFORMATION *information;
};

extern struct ATA_DEVICE ATA_DEVICE0;
extern struct ATA_DEVICE ATA_DEVICE1;

/*
 * プロトタイプ宣言
 */
void INITIALIZE_ATA_DEVICE(void);

i32_t write_ata_sector(struct ATA_DEVICE *device, u32_t lba, void *buffer, i32_t count);
i32_t read_ata_sector(struct ATA_DEVICE *device, u32_t lba, void *buffer, i32_t count);

i32_t wait_bsy_clear(struct ATA_DEVICE *device);
i32_t wait_set_drdy(struct ATA_DEVICE *device);

i32_t ata_select_device(struct ATA_DEVICE *device);


#define __SELECT_DEVICE_MACRO(device)  do {          \
    if (ata_select_device(device) < 0) {  			\
        return -1;                     				\
    }                                  				\
} while (0)


#endif
