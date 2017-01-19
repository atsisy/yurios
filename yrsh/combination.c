#include "../include/kernel.h"
#include "../include/sh.h"

/*
 *ファイルに対する操作のフラグ
 */
#define __CREATE_NEW__   0x01
#define __FILE_ADD__     0x02
#define __UNDEFINED_CC__ 0x04

static u8_t CreateNewOrAdd(char *str);

/*
 *=======================================================================================
 *SearchCombinationCommand関数
 *コマンド組み合わせの修飾子を探す関数
 *=======================================================================================
 */
i32_t SearchCombinationCommand(char *command){
	u32_t i = 0;
	u8_t flag;
	char alphabet = 0;

	while((alphabet = command[i])){
		switch(alphabet){
		case '>':
			if((flag = CreateNewOrAdd(command + i)) & __CREATE_NEW__)
				Redirect(command);
			else if(flag & __FILE_ADD__)
				/*
				RedirectFAdd(command);
				*ファイル追記形式ののリダイレクト処理
				*/

			break;
		case '|':
			/*
			 *パイプ処理
			 */
			break;
		}

		if(!alphabet)
			break;
		i++;
	}

	return -1;
}

/*
 *=======================================================================================
 *ファイル新規作成のリダイレクトか、ファイルに追記する形式のリダイレクトかを判定する関数
 *=======================================================================================
 */
static u8_t CreateNewOrAdd(char *str){
	if(str[1] == '>')
		return __FILE_ADD__;
	else if(str[1] == ' ')
		return __CREATE_NEW__;
	return __UNDEFINED_CC__;
}

