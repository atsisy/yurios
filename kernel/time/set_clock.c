#include "../../include/kernel.h"

static void access_rct(u8_t data, u8_t flag);

/*
 *=======================================================================================
 *do_setclock関数
 *現在時刻をRCTにアクセスして書き込む関数
 *フラグによって何をするか決める
 *=======================================================================================
 */
void do_setclock(u16_t value, u8_t flag){

	if(flag & __SECOND__){
		/*
		 *秒を取得
		 */
		access_rct(value, 0x00);
	}else if(flag & __MINUTE__){
		/*
		 *分を取得
		 */
		access_rct(value, 0x02);
	}else if(flag & __HOUR__){
		/*
		 *時間を取得
		 */
		access_rct(value, 0x04);
	}else if(flag & __DAY__){
		/*
		 *日を取得
		 */
		access_rct(value, 0x07);
	}else if(flag & __MONTH__){
		/*
		 *月を取得
		 */
		access_rct(value, 0x08);
	}else if(flag & __YEAR__){
		/*
		 *年を取得
		 *ここだけ16bit分計算
		 */
		access_rct(value%100, 0x09);
		access_rct(value/100, 0x32);
	}

	return;
}

/*
 *=======================================================================================
 *access_rct関数
 *RCTにアクセスして時間を書き込む関数
 *=======================================================================================
 */
static void access_rct(u8_t value, u8_t flag) {
      io_out8(0x70, flag);
	io_out8(0x71, value);
}
