#include "../../include/ata.h"
#include "../../include/types.h"
#include "../../include/kernel.h"

i32_t ata_select_device_ext(struct ATA_DEVICE *device, u8_t ext_flags);

/*
 *=======================================================================================
 *prepare_wr関数
 *ATAデバイスの読み書きをする準備をする関数
 *引数
 *i32_t device
 *=>デバイス選択
 *u32_t lba
 *=>LBA方式での読み書き位置
 *u32_t count
 *=>読み書きするセクタ数
 *返り値
 *終了状況
 *=======================================================================================
 */
i32_t prepare_wr(struct ATA_DEVICE *device, u32_t lba, i32_t count){

    u8_t sector_num, cyl_lo, cyl_hi, dev_head;

    if (device->information->is_support_lba) {

        sector_num   = lba & 0xFF;
        cyl_lo   = (lba >> 8) & 0xFF;
        cyl_hi   = (lba >> 16) & 0xFF;
        dev_head = ((lba >> 24) & 0xF) | __ATA_DH_LBA__;

    } else {

        u16_t h = device->information->current_head_num;
        u16_t s = device->information->current_sector_num;

        sector_num   = (lba % s) + 1;
        u16_t cyl = lba / (s * h);
        cyl_lo   = cyl & 0xff;
        cyl_hi   = (cyl >> 8) & 0xff;
        dev_head = (lba / s) % h;
    }

    if(ata_select_device_ext(device, dev_head) < 0)
        return -1;

    io_out8(__ATA_PORT_SECTOR_CNT(device), count);
    io_out8(__ATA_PORT_SECTOR_NO(device),  sector_num);
    io_out8(__ATA_PORT_CYL_LO(device),     cyl_lo);
    io_out8(__ATA_PORT_CYL_HI(device),     cyl_hi);

    return 0;
}
