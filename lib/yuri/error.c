#include "../../include/string.h"
#include "../../include/sh.h"

/*
 *=======================================================================================
 *pError関数
 *標準エラーに出力する関数
 *引数
 *char *message
 *エラーとして出力するメッセージ
 *=======================================================================================
 */
void pError(char *message){
	/*
	 *一回クリア
	 */
	boxfill8(binfo->vram, binfo->scrnx, BLACK,
		   length << 3, input_y + (indent << 4), (length << 3)+(strlen(message) << 3), (input_y + (indent << 4)+16));

	/*
	 *実際に表示する
	 */
	putfonts8_asc(binfo->vram, binfo->scrnx, length << 3, input_y + (indent << 4), COL8_FFFFFF, message);

	indent_shell();

	return;
}
