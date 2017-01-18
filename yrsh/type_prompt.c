#include "../include/kernel.h"
#include "../include/sh.h"


/*
 *ユーザからの入力を受ける関数
 */
void type_prompt(char *dst, int buffer_limit){

	struct Process *proc = task_now();
	int i;
	/*
	 *サブキーのフラグ変数
	 */
	int key_shift = 0, key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1, key_ctrl = 0;
	/*
	 *直前に受け取った1文字だけを保持しておく変数
	 */
	char input_char;

	struct QUEUE keycmd;
	int keycmd_buf[32];
	queue_init(&keycmd, 32, keycmd_buf, 0);

	for(;;){

		if(!queue_size(proc->irq)){
			/*
			 *割り込み来ないから寝る
			 */
			task_sleep(proc);
			io_sti();
		}else{
			/*
			 *割り込みきたー
			 */
			i = queue_pop(proc->irq);   //なんの割り込みか確認
			io_sti();
			if(i >= 256 && i <= 511) { //キーボードからの割り込みだったー！！

				if(i == 256+0x2e && key_ctrl == 1){	//Ctrl+Cなので強制終了処理
					io_cli();	//強制終了中にプロセスが変わると面倒なことになるので、割り込み禁止にする
					/*
					  強制終了処理
					*/
					io_sti();	//割り込み許可
				}
				io_cli();

				if(i  == 0x1c + 256){ //Enterキーの処理
					increase_length();
					erase_a_alphabet();
					dst[length-1] = '\0';
					/*
					 *Enterキー押したらコマンドに行くからね
					 */
					indent_shell();
					return;
				}else if(i == 256 + 0x0e){	//BackSpaceキーの処理
					if(length >= 2){	//">"これを消さないようにする
						increase_length();
						erase_a_alphabet();	//裏でlengthがデクリメントされている
						erase_a_alphabet();
						dst[length-1] = '\0';
					}
				}else if (i < 256 + 0x80){
					if(key_shift == 0){
						input_char = keys0[i - 256];
					}else{
						input_char = keys1[i - 256];
					}
					if ('A' <= input_char && input_char <= 'Z'){	/* 入力文字がアルファベット */
						if (((key_leds & 4) == 0 && key_shift == 0) ||
						    ((key_leds & 4) != 0 && key_shift != 0)) {
							input_char += 0x20;	/* 大文字を小文字に変換 */
						}
						dst[length-1] = input_char;
						sent_command[length] = '\0';
						put_char(input_char);
					}else if(i == 256 + 0x39){	//スペースキーをおした時の処理
						dst[length-1] = ' ';
						dst[length]   = '\0';
						put_char(' ');
					}else if('!' <= input_char && input_char <= '~'){
						dst[length-1] = input_char;
						dst[length]   = '\0';
						put_char(input_char);
					}
				}

				if(i == 256 + 0x0f){	//Tabキーの処理
          				putfonts8_asc(binfo->vram, binfo->scrnx, length << 3, input_y + ((indent)*16), COL8_FFFFFF, " ");
          				sent_command[length-1] = input_char;
          				sent_command[length] = 0;
          				length++;
				}
				
				if(i == 256 + 0x2a){	//左shiftの処理(おした時)
					key_shift |= 1;
				}
				if(i == 256 + 0x36){	//右shiftの処理(おした時)
					key_shift |= 2;
				}
				if(i == 256 + 0xaa){	//左shiftの処理(離した時)
					key_shift &= ~1;
				}
				if(i == 256 + 0xb6){	//右shiftの処理(離した時)
					key_shift &= ~2;
				}
				if(i == 256 + 0x1d){	//Ctrlキーを押したときの処理
					key_ctrl = 1;
				}
				if(i == 256 + 0x9d){
					key_ctrl = 0;
				}
				if (i == 256 + 0x3a) {	/* CapsLock */
					key_leds ^= 4;
					queue_push(&keycmd, KEYCMD_LED);
					queue_push(&keycmd, key_leds);
				}
				if (i == 256 + 0x45) {	/* NumLock */
					key_leds ^= 2;
					queue_push(&keycmd, KEYCMD_LED);
					queue_push(&keycmd, key_leds);
				}
				if (i == 256 + 0x46) {	/* ScrollLock */
					key_leds ^= 1;
					queue_push(&keycmd, KEYCMD_LED);
					queue_push(&keycmd, key_leds);
				}
				if(i == 256 + 0x3b){	//F1キーの処理	YURIOSではマルチCUIモード
					shell_mode = DUAL;
					ylsh_clear();
					put_char('%');
					increase_length();
					MAX_SCROLL /= 2;
					draw_hline(binfo->vram, binfo->scrnx, binfo->scrny,
						     binfo->scrnx-RUNNING_TASK_SPACE, binfo->scrny >> 1);
				}

				if (i == 256 + 0xfa) {	/* キーボードがデータを無事に受け取った */
					keycmd_wait = -1;
				}
				if (i == 256 + 0xfe) {	/* キーボードがデータを無事に受け取れなかった */
					wait_KBC_sendready();
					io_out8(PORT_KEYDAT, keycmd_wait);
				}
			}
			io_sti();
		}
	}
	indent_shell();
}

/*
 *ユーザからの入力を受ける関数
 */
void ntype_prompt(char *dst, int buffer_limit){

	struct Process *proc = task_now();
	int i;
	/*
	 *サブキーのフラグ変数
	 */
	int key_shift = 0, key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1, key_ctrl = 0;
	/*
	 *直前に受け取った1文字だけを保持しておく変数
	 */
	char input_char;
	u16_t index = 0;

	struct QUEUE keycmd;
	int keycmd_buf[32];
	queue_init(&keycmd, 32, keycmd_buf, 0);

	//dstを0クリア
	zeroclear_8array(dst, 1024 << 2);
	for(;;){

		if(!queue_size(proc->irq)){
			/*
			 *割り込み来ないから寝る
			 */
			task_sleep(proc);
			io_sti();
		}else{
			/*
			 *割り込みきたー
			 */
			i = queue_pop(proc->irq);   //なんの割り込みか確認
			io_sti();
			if(i >= 256 && i <= 511) { //キーボードからの割り込みだったー！！

				if(buffer_limit <= index)
					continue;
				
				if(i == 256+0x2e && key_ctrl == 1){	//Ctrl+Cなので強制終了処理
					io_cli();	//強制終了中にプロセスが変わると面倒なことになるので、割り込み禁止にする
					/*
					  強制終了処理
					*/
					io_sti();	//割り込み許可
				}
				io_cli();

				if(i  == 0x1c + 256){ //Enterキーの処理
				      dst[index] = '\0';
					increase_length();
					erase_a_alphabet();
					indent_shell();
					return;
				}else if(i == 256 + 0x0e){	//BackSpaceキーの処理
					if(length >= 2){	//">"これを消さないようにする
						dst[index] = '\0';
						increase_length();
						erase_a_alphabet();
						erase_a_alphabet();
						index--;
					}
				}else if (i < 256 + 0x80){
					if(key_shift == 0){
						input_char = keys0[i - 256];
					}else{
						input_char = keys1[i - 256];
					}
					if ('A' <= input_char && input_char <= 'Z'){	/* 入力文字がアルファベット */
						if (((key_leds & 4) == 0 && key_shift == 0) ||
						    ((key_leds & 4) != 0 && key_shift != 0)) {
							input_char += 0x20;	/* 大文字を小文字に変換 */
						}
						dst[index] = input_char;
						index++;
						put_char(input_char);
					}else if(i == 256 + 0x39){	//スペースキーをおした時の処理
						dst[index] = ' ';
						index++;
						put_char(' ');
					}else if('!' <= input_char && input_char <= '~'){
						dst[index] = input_char;
						put_char(input_char);
						index++;
					}
				}

				if(i == 256 + 0x0f){	//Tabキーの処理
          				/*
					 *自由に設定
					 */
				}
				
				if(i == 256 + 0x2a){	//左shiftの処理(おした時)
					key_shift |= 1;
				}
				if(i == 256 + 0x36){	//右shiftの処理(おした時)
					key_shift |= 2;
				}
				if(i == 256 + 0xaa){	//左shiftの処理(離した時)
					key_shift &= ~1;
				}
				if(i == 256 + 0xb6){	//右shiftの処理(離した時)
					key_shift &= ~2;
				}
				if(i == 256 + 0x1d){	//Ctrlキーを押したときの処理
					key_ctrl = 1;
				}
				if(i == 256 + 0x9d){
					key_ctrl = 0;
				}
				if (i == 256 + 0x3a) {	/* CapsLock */
					key_leds ^= 4;
					queue_push(&keycmd, KEYCMD_LED);
					queue_push(&keycmd, key_leds);
				}
				if (i == 256 + 0x45) {	/* NumLock */
					key_leds ^= 2;
					queue_push(&keycmd, KEYCMD_LED);
					queue_push(&keycmd, key_leds);
				}
				if (i == 256 + 0x46) {	/* ScrollLock */
					key_leds ^= 1;
					queue_push(&keycmd, KEYCMD_LED);
					queue_push(&keycmd, key_leds);
				}
				if(i == 256 + 0x3b){	//F1キーの処理	YURIOSではマルチCUIモード
					shell_mode = DUAL;
					ylsh_clear();
					put_char('%');
					increase_length();
					MAX_SCROLL /= 2;
					draw_hline(binfo->vram, binfo->scrnx, binfo->scrny,
						     binfo->scrnx-RUNNING_TASK_SPACE, binfo->scrny >> 1);
				}

				if (i == 256 + 0xfa) {	/* キーボードがデータを無事に受け取った */
					keycmd_wait = -1;
				}
				if (i == 256 + 0xfe) {	/* キーボードがデータを無事に受け取れなかった */
					wait_KBC_sendready();
					io_out8(PORT_KEYDAT, keycmd_wait);
				}
			}
			io_sti();
		}
	}
	indent_shell();
}
