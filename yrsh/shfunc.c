#include "../include/sh.h"
#include "../include/kernel.h"

/*
 *=======================================================================================
 *increase_indent関数
 *シェルをインデントさせる関数
 *引数返り値なし
 *=======================================================================================
 */
void indent_shell(void){

	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;

	/*
	 *スクロールが必要な場合
	 */
	if(indent > MAX_SCROLL-2){
		switch(shell_mode){
		case 0:	//SINGLEモード
			scroll(32);
			break;
		case 1:     //デュアルモード
			multi_shellscroll(binfo, 32, 0, MAX_SCROLL-1);
			boxfill8(binfo->vram, binfo->scrnx,
				   BLACK, 0, (MAX_SCROLL-1) << 4, binfo->scrnx-151, ((MAX_SCROLL) << 4));
			break;
		default:
			/*
			 *シェルのモードが狂っている
			 */
			print("ERROR");
		}
		return;
	}

	/*
	 *インクリメント
	 */
	indent++;
	/*
	 *書き込みのX位置は初期化
	 */
	length = 0;

	return;
}

/*
 *=======================================================================================
 *increase_length関数
 *シェルへの書き込みのX位置を一つ右へずらす関数
 *引数返り値なし
 *=======================================================================================
 */
void increase_length(void){
	/*
	 *X位置をインクリメント
	 */
	length++;
	return;
}

/*
 *=======================================================================================
 *erase_a_alphabet関数
 *現在のX位置の一つ前にいる文字を消す(シェル的に)関数
 *引数返り値なし
 *=======================================================================================
 */
void erase_a_alphabet(void){
	/*
	 *X位置をデクリメント
	 */
	length--;
	/*
	 *シェル上から消去
	 */
	boxfill8(binfo->vram, binfo->scrnx, BLACK,
		   length << 3, (indent << 4)+input_y, (length << 3)+8, (indent << 4)+input_y+16);

	return;
}
