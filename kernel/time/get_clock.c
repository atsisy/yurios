#include "../../include/kernel.h"

static char access_rct(char flag);
static char translate_time(char time);
static u16_t translate_timeh(u16_t time);

/*
 *=======================================================================================
 *do_gettime関数
 *現在時刻をRCTにアクセスして得る関数
 *フラグによって何を返すか決める
 *=======================================================================================
 */
u16_t do_gettime(char flag){

	if(flag & __SECOND__){
		/*
		 *秒を取得
		 */
		return translate_time(access_rct(0x00));
	}else if(flag & __MINUTE__){
		/*
		 *分を取得
		 */
		return translate_time(access_rct(0x02));
	}else if(flag & __HOUR__){
		/*
		 *時間を取得
		 */
		return translate_time(access_rct(0x04));
	}else if(flag & __DAY__){
		/*
		 *日を取得
		 */
		return translate_time(access_rct(0x07));
	}else if(flag & __MONTH__){
		/*
		 *月を取得
		 */
		return translate_time(access_rct(0x08));
	}else if(flag & __YEAR__){
		/*
		 *年を取得
		 *ここだけ16bit分計算
		 */
		return translate_timeh(access_rct(0x09) | (access_rct(0x32) << 8));
	}

	/*
	 *失敗
	 */
	return FAILURE;
}

/*
 *=======================================================================================
 *access_rct関数
 *RCTにアクセスして時間を得る関数
 *=======================================================================================
 */
static char access_rct(char flag) {
	io_out8(0x70, flag);
	return io_in8(0x71);
}

/*
 *=======================================================================================
 *translate_time関数
 *RCTの変な16進数えで見るとOKとかいういみわからん方式から通常の10進数に変換する関数
 *=======================================================================================
 */
static char translate_time(char time) {
	return  (time & 0x0f) + (time >> 4)*10;
}

/*
 *=======================================================================================
 *translate_timeh関数
 *RCTの変な16進数えで見るとOKとかいういみわからん方式から通常の10進数に変換する関数
 *=======================================================================================
 */
static u16_t translate_timeh(u16_t time) {
	return (time & 0x000f)
		+
		(((time & 0x00f0) >> 4)*10)
		+
		(((time & 0x0f00) >> 8)*100)
		+
		(((time & 0xf000) >> 12)*1000);
}
