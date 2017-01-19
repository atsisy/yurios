#include "../include/sh.h"
#include "../include/string.h"

void yrsh_interpreter(char *command);

/*
 *=======================================================================================
 *Redirect関数
 *リダイレクトを処理する関数
 *引数
 *char *command
 *シェルに打ち込まれたコマンド
 *=======================================================================================
 */
void Redirect(char *command){
	/*
	 * > までの文字オフセットを計算
	 */
      int offset = SearchStringFirst(command, '>');

	/*
	 *計算したオフセットからコマンド用バッファを確保
	 */
	char *red_pare = (char *)memory_alloc(memman, offset - 1);

	zeroclear_8array(red_pare, offset - 1);

	/*
	 *計算したオフセットからコマンドを切り取り
	 */
	memcpy(red_pare, command, offset - 2);

	//インタプリタに投げる
	yrsh_interpreter(red_pare);

	//後方のファイル名のファイルに対しリダイレクト処理
	RedirectCreateFile(command + offset + 1);

	//メモリ解放
	memory_free(memman, (u32_t)red_pare, offset - 1);
}

/*
 *=======================================================================================
 *RedirectFAdd関数
 *リダイレクトを処理する関数
 *引数
 *char *command
 *シェルに打ち込まれたコマンド
 *=======================================================================================
 */
void RedirectFAdd(char *command){
	/*
	 * >> までの文字オフセットを計算
	 *※注意※
	 *この関数はすでにファイル追加形式のリダイレクトが投げられているとわかった状況で呼び出されます
	 */
      int offset = SearchStringFirst(command, '>');

	//>>は2文字なのでインクリメント
	offset++;

	/*
	 *計算したオフセットからコマンド用バッファを確保
	 */
	char *red_pare = (char *)memory_alloc(memman, offset - 2);

	zeroclear_8array(red_pare, offset - 2);

	/*
	 *計算したオフセットからコマンドを切り取り
	 */
	memcpy(red_pare, command, offset - 3);

	//インタプリタに投げる
	yrsh_interpreter(red_pare);

	//後方のファイル名のファイルに対しリダイレクト処理
	RedirectFileAddSub(command + offset + 1);

	//メモリ解放
	memory_free(memman, (u32_t)red_pare, offset - 1);
}
