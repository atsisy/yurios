#include "../include/bootpack.h"
#include "../include/sh.h"
#include "../include/string.h"
#include "../include/value.h"
#include "../include/ata.h"
#include "../include/yrfs.h"

int do_open(char *pathname, u32_t flags);
void put_char(char ch);

int length, indent, MAX_SCROLL;
short input_y;
enum SHELL_MODE shell_mode;
/*
	シェルモードについて
	0:ノーマルモード(1つ)
	1:デュアルモード(２つ)
	2:クアッドモード(4つ)
*/

void ylsh_cursor(void);

short enter_flag;

struct Process *ylsh_cursor_timer;
/*
	enter_flagが0=>押されていない
	enter_flagが1=>押された
*/

/*
 *void shell_init(void)
 *シェルの初期化関数
 *シェル内部に使うであろう変数を初期化したりする
 */
void shell_init(void){

	/*
	 *パラメータ初期化
	 */
	input_y = 32;
	length = 0;
	indent = 1;
	enter_flag = 0;
	MAX_SCROLL = (binfo->scrny >> 4)-2;
	draw_vline(binfo->vram, binfo->scrnx,
		 binfo->scrny, binfo->scrnx-RUNNING_TASK_SPACE, binfo->scrny);

	/*
	 *シェル起動時のモードはシングルモード
	 */
	shell_mode = SINGLE;

	/*
	 *カーソルのスレッドの準備を行う
	 */
	ylsh_cursor_timer = task_alloc("shell cursor");
	ylsh_cursor_timer->tss.eip = (int) &ylsh_cursor;
	ylsh_cursor_timer->tss.esp = memory_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
	ylsh_cursor_timer->tss.es = 1 * 8;
	ylsh_cursor_timer->tss.cs = 2 * 8;
	ylsh_cursor_timer->tss.ss = 1 * 8;
	ylsh_cursor_timer->tss.ds = 1 * 8;
	ylsh_cursor_timer->tss.fs = 1 * 8;
	ylsh_cursor_timer->tss.gs = 1 * 8;
	//*((int *) (ylsh_cursor_timer->tss.esp + 4)) = 0;

	/*
	 *一足先にカーソルを起動1
	 */
	task_run(ylsh_cursor_timer, 2, 2);
}

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

		if(!queue_size(&(proc->irq))){
			/*
			 *割り込み来ないから寝る
			 */
			task_sleep(proc);
			io_sti();
		}else{
			/*
			 *割り込みきたー
			 */
			i = queue_pop(&(proc->irq));   //なんの割り込みか確認
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
}

void shell_master(void){

	char command[1024];

	struct Process *me = task_now();	//自分自身を指すプロセス構造体
	struct Process *child_proc;				//シェルから生成されるプロセスを指すためのプロセス構造体

	int shell_buf[128];								//シェルに来るシグナルや割り込み情報をためておくバッファ

	queue_init(&(me->irq), 128, shell_buf, me);	//シェル用FIFOを初期化

	/*
	  キーボードドライバが情報を流し込んでくるキーバッファを
	  シェルのバッファ(me->irq)に変更する
	*/
	ch_keybuf(&(me->irq));

	int *fat = (int *)memory_alloc_4k(memman, 4 * 2880);
	char part[1024], copied_str[1024];
	readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));

	for(;;){
		io_cli();

		/*
		 *ユーザの入力待ち
		 */
		type_prompt(command, 1024);

		/*
		 *処理を実行する際にカーソルが点滅するのはおかしいので一旦寝てもらう
		 */
		task_sleep(ylsh_cursor_timer);

  		if(command[0] ==  '\0'){   //なにも入力されずEnterキーを押した場合
    			indent_shell();
    			put_char('%');
			enter_flag = 0;
			length = 1;
    			continue;
		}

		strcpy(copied_str, command, strlen(command)+1);

		/*
		 *コマンド打ったあと一個下の行を渡す
		 */
		indent_shell();

		string_getNext(command, part);	//受け取ったコマンドを分割して先頭の文字列のみを取り出す

		/*
		 *コマンドに合わせて処理を実行する
		 */
		if(strcmp(part, "memory")){
			/*
			 *空きメモリ容量を表示するコマンド
			 */
			command_memory();

		}else if(strcmp(part, "echo")){
			/*
			 *UNIXコマンドのechoコマンド的なやつ
			 */
			command_echo(command);
		}else if(strcmp(part, "history")){
			/*
			 *そのまんまhistoryコマンド
			 */
			command_history();
		}else if(strcmp(part, "ls")){
			/*
			 *lsもどき
			 */
			command_ls();
		}else if(strcmp(part, "cat")){
			/*
			 *ファイルの内容を表示
			 */
			command_cat(command);
		}else if(strcmp(command, "clear")){
			/*
			 *シェルの画面を新しい画面に置き換えるコマンド
			 *UNIXのclearと同じく
			 */
			ylsh_clear();
		}else if(strcmp(command, "open")){

			do_open("SampleFile", __O_CREAT__);
			struct i_node inode;
			iread(&inode, 0);
			puts(inode.file_name);

			
		}else if(strcmp(command, "atainit")){

			
			char write_buf[256] = "YURI";
			write_buf[255] = '\0';
			char read_buf[256];
			read_buf[0] = 'N';
			write_ata_sector(&ATA_DEVICE0, 1, write_buf, 1);
			read_ata_sector(&ATA_DEVICE0, 1, read_buf, 1);
			puts(read_buf);

			
		}else if(strcmp(command, "lscpu")){
			command_lscpu();
		}else if(do_shell_app(fat, copied_str) == 0){
			//対応するコマンドではなく、さらにアプリケーションでもない場合
			/*
			 *外部アプリケーション実行
			 *もしくはそんなコマンドねーよ的な
			 */
			print("Sorry. I Could not recognize '");
			length += 30;
			print(copied_str);
			length += strlen(copied_str);
			print("'.");
			indent_shell();
		}
		/*
		 *次のコマンドを受け付けるための準備
		 */
		put_char('%');

		/*
		 *何らかの処理中にコンソールのカーソルが動いてるのはおかしいのでストップしていたカーソル点滅スレッドを再開
		 */
		task_run(ylsh_cursor_timer, -1, 0);

	}

}

/*
 *clearコマンドのための関数
 */
void ylsh_clear(void){
	/*
	 *コマンド領域をすべて黒で塗りつぶす
	 */
	boxfill8(binfo->vram, binfo->scrnx,
		 BLACK, 0, 0, binfo->scrnx-151, binfo->scrny);

	/*
	 *入力位置を初期化
	 */
	indent = length = input_y = 0;
}

/*
 *ylshのカーソルを表示する関数（スレッド）
 */
void ylsh_cursor(void) {
	/*
	 *変数宣言
	 */
	int flag = 0;         //現在カーソルが光っているかフラグ
	struct QUEUE fifo;
	struct TIMER *timer;
	int fifo_buf[128];
	int wrote_x, wrote_y;

	/*
	 *タイマの設定（0.5秒に一回点滅するくらいがいいと思ふ）
	 */
	queue_init(&fifo, 128, fifo_buf, 0);
	timer = timer_alloc();
	timer_init(timer, &fifo, 10);	//割り込みが来たら10が返る
	timer_settime(timer, 500);    //500msに設定

	boxfill8(binfo->vram, binfo->scrnx, BLACK,
		 length << 3, input_y + (indent << 4), (length << 3)+4, (input_y + (indent << 4)+16));

	while(1){
		if(!queue_size(&fifo)){
			/*
			 *割り込み来てない
			 */
			io_stihlt();
		}else if(queue_pop(&fifo) == 10){
			/*
			 *割り込みきたー
			 */
			if(flag == 0){	//点灯していない
				/*
				 *描画処理
				 */
				wrote_x = length << 3;
				wrote_y = input_y + (indent << 4);
				boxfill8(binfo->vram, binfo->scrnx, BLACK, wrote_x,
					 wrote_y, wrote_x+4, wrote_y+16);
				flag = 1;
			}else{	//点灯している
				/*
				 *描画処理
				 */
				boxfill8(binfo->vram, binfo->scrnx,COL8_FFFFFF,
					 length << 3, input_y + (indent << 4), (length << 3)+4, (input_y + (indent << 4)+16));
				flag = 0;
			}
			/*
			 *タイマ再設定
			 */
			timer_settime(timer, 500);
		}
	}
}

/*
 *コマンドを何度か打ってこれ以上下へいけなくなったときにシェルの画面をスクロールする関数
 */
void scroll(struct BOOTINFO *binfo, int height){
	/*
	 *スクロールに必要なパラメータの取得、計算
	 */
	int i, textzone_x = binfo->scrnx-151, scrnx = binfo->scrnx, scrny = binfo->scrny;
	int scrnxy = scrnx * scrny;
	int copy_from = scrnx * height;

	/*
	 *塗替え処理
	 */
	for(i = 0;i < scrnxy;i++){
		if((i % scrnx) > textzone_x){
			i += 149;
		}else{
			binfo->vram[i] = binfo->vram[i+copy_from];
		}
	}

	/*
	 *入力位置を適宜決定
	 */
	indent -= (height >> 4);
	length = 0;

	return;
}

/*
 *複数モードのとき
 */
void multi_shellscroll(struct BOOTINFO *binfo, int height, int top, int under){
	int i, textzone_x = binfo->scrnx-151, scrnx = binfo->scrnx;
	int under_addr = under*scrnx;
	int copy_from = scrnx * height;
	
	for(i = top*scrnx;i < under_addr;i++){
		if((i % scrnx) > textzone_x){
			i += 149;
		}else{
			binfo->vram[i] = binfo->vram[i+copy_from];
		}
	}
	indent -= (height >> 4);
	length = 0;
	return;
}

/*
 *外部アプリケーションを実行するための関数
 */
int do_shell_app(int *fat, char *command){
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct FileInfo *finfo;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
	char *file_name, *p, *q;
	struct Process *me = task_now();
	int i, seg_size, data_size, esp, dathrb;

	for(i = 0;i < 13;i++){
		if(command[i] <= ' '){
			break;
		}
		file_name[i] = command[i];
	}
	file_name[i] = '\0';
	finfo = file_search(file_name, (struct FileInfo *)(ADR_DISKIMG+0x002600), 224);	//なんかバイナリエディタで確認したら0x2600から始まってた
	if(finfo == 0 && file_name[i-1] != '.'){
		//ただのファイル名では見つからなかったため.ylつきで検索する
		file_name[i] = '.';
		file_name[i+1] = 'y';
		file_name[i+2] = 'x';
		file_name[i+3] = '\0';
		finfo = file_search(file_name, (struct FileInfo *)(ADR_DISKIMG+0x002600), 224);
	}

	if(finfo != 0){	//ファイルを発見した場合
		/* ファイルが見つかった場合 */
		p = (char *) memory_alloc_4k(memman, finfo->size);
		loadfile(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));
		if (finfo->size >= 36 && strcmp(p + 4, "yrex") == 1 && *p == 0x00){
			seg_size 	= *((int *) (p + 0x0000));
			esp    		= *((int *) (p + 0x000c));
			data_size = *((int *) (p + 0x0010));
			dathrb 		= *((int *) (p + 0x0014));
			q = (char *) memory_alloc_4k(memman, seg_size);
			*((int *)0xfe8) = (int)q;
			set_segmdesc(gdt + 1003, finfo->size - 1, (int) p, AR_CODE32_ER+0x60);
			set_segmdesc(gdt + 1004, seg_size - 1,    (int) q, AR_DATA32_RW+0x60);
			for (i = 0; i < data_size; i++){
				q[esp+i] = p[dathrb+i];
			}
			start_app(0x1b, 1003 * 8, esp, 1004 * 8, &(me->tss.esp0));
			memory_free_4k(memman, (int)q, seg_size);
		}else{
			print("yuri executable file format error.");
		}
		memory_free_4k(memman, (int) p, finfo->size);

		return 1;
	}
	//ファイルが見つからなかった場合
	return 0;
}

struct FileInfo *file_search(char *name, struct FileInfo *finfo, int max){
	/*
	 *forループに使う変数
	 *C++の方がやりやすいわぁ
	 */
	int i, j;

	char s[12];

	/*
	 *クリア
	 */
	for (j = 0; j < 11; j++) {
		s[j] = ' ';
	}
	j = 0;
	for (i = 0; name[i] != 0; i++){
		if (j >= 11) {
			return 0; /* 見つからなかった */
		}

		if (name[i] == '.' && j <= 8) {
			j = 8;
		}else{
			s[j] = name[i];
			if ('a' <= s[j] && s[j] <= 'z') {
				/* 小文字は大文字に直す */
				s[j] -= 0x20;
			}
			j++;
		}
	}

	for (i = 0; i < max; ) {
		if (finfo->name[0] == 0x00) {
			break;
		}
		if ((finfo[i].type & 0x18) == 0) {
			for (j = 0; j < 11; j++) {
				if (finfo[i].name[j] != s[j]) {
					goto next;
				}
			}
			return finfo + i; /* ファイルが見つかった */
		}
	next:
		i++;
	}
	return 0; /* 見つからなかった */
}
