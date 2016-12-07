#include "../../include/bootpack.h"
#include "../../include/sh.h"
#include "../../include/string.h"
#include "../../include/value.h"

/*
 *=======================================================================================
 *print関数
 *文字列のみを標準出力できる関数
 *引数
 *char *str
 *=>表示したい文字列
 *=======================================================================================
 */
void print(char *str){
	/*
	 *一回クリア
	 */
	boxfill8(binfo->vram, binfo->scrnx, BLACK,
		 length << 3, input_y + (indent << 4), (length << 3)+(strlen(str) << 3), (input_y + (indent << 4)+16));

	/*
	 *実際に表示する
	 */
	putfonts8_asc(binfo->vram, binfo->scrnx, length << 3, input_y + (indent << 4), COL8_FFFFFF, str);

	length += strlen(str);
	
	return;
}

/*
 *=======================================================================================
 *puts関数
 *文字列のみを標準出力できる関数
 *自動でインデントされる
 *引数
 *char *str
 *=>表示したい文字列
 *=======================================================================================
 */
void puts(char *str){
	/*
	 *一回クリア
	 */
	boxfill8(binfo->vram, binfo->scrnx, BLACK,
		 length << 3, input_y + (indent << 4), (length << 3)+(strlen(str) << 3), (input_y + (indent << 4)+16));

	/*
	 *実際に表示する
	 */
	putfonts8_asc(binfo->vram, binfo->scrnx, length << 3, input_y + (indent << 4), COL8_FFFFFF, str);

	indent_shell();

	return;
}
/*
 *=======================================================================================
 *put_char関数
 *一文字だけ表示する関数
 *引数
 *char ch
 *=>表示したい文字
 *=======================================================================================
 */
void put_char(char ch){

	extern char dfont[__DFONT_ELEMENTS__];	//フォントデータ

	boxfill8(binfo->vram, binfo->scrnx, BLACK,
		   length << 3, input_y + (indent << 4), (length << 3)+8, (input_y + (indent << 4)+16));

	/*
	 *一文字だけ表示
	 */
	putfont8(binfo->vram, binfo->scrnx, length << 3, input_y+(indent << 4), COL8_FFFFFF, dfont + ch * 16);

	/*
	 *シェルの内部変数をインクリメント
	 */
	length++;

	return;
}

/*
 *=======================================================================================
 *print_value関数
 *数値のみを表示するためだけの関数
 *引数
 *int value
 *=>表示したい数値
 *int x
 *=>表示したいx座標
 *int y
 *=>表示したいy座標
 *=======================================================================================
 */
void print_value(int value, int x, int y){

	/*
	 *数値を文字列に変換したあとにその文字列を保持しておくための変数
	 */
	char str[30];

	/*
	 *数値を文字列に変換
	 */
	sprintf(str, "%d", value);

	/*
	 *実際に表示する
	 */
	putfonts8_asc(binfo->vram, binfo->scrnx, x, y, COL8_FFFFFF, str);

	return;
}
