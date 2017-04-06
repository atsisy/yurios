#include "../include/kernel.h"
#include "../include/sh.h"
#include "../include/string.h"
#include "../include/value.h"
#include "../include/ata.h"
#include "../include/yrfs.h"
#include "../include/util_macro.h"

extern struct Directory CurrentDirectory;

void put_char(char ch);
static void put_first_str();
void yrsh_interpreter(char *command);

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

void yrsw_main();

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
	input_y = 0;
	length = 0;
	indent = 1;
	enter_flag = 0;
	MAX_SCROLL = (binfo->scrny >> 4)-2;

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
 *=======================================================================================
 *shell_master関数
 *シェルのメイン関数
 *=======================================================================================
 */
void shell_master(void){

	char *command = (char *)memory_alloc(memman, 1024 << 2);

	struct Process *me = task_now();	//自分自身を指すプロセス構造体

	int shell_buf[128];								//シェルに来るシグナルや割り込み情報をためておくバッファ

	//history_init();

	queue_init(me->irq, 128, shell_buf, me);	//シェル用FIFOを初期化

	/*
	  キーボードドライバが情報を流し込んでくるキーバッファを
	  シェルのバッファ(me->irq)に変更する
	*/
	ch_keybuf(me->irq);

	for(;;){
		put_first_str();
		io_cli();

		/*
		 *ユーザの入力待ち
		 */
		ntype_prompt(command, 1024, 2);

		/*
		 *処理を実行する際にカーソルが点滅するのはおかしいので一旦寝てもらう
		 */
		task_sleep(ylsh_cursor_timer);

  		if(command[0] ==  '\0'){   //なにも入力されずEnterキーを押した場合
    			put_char('%');
			enter_flag = 0;
			length = 1;
			task_run(ylsh_cursor_timer, -1, 0);
    			continue;
		}

		yrsh_interpreter(command);

		/*
		 *次のコマンドを受け付けるための準備
		 */

		//出力ストリームを初期化
		ResetOutputStream();

		//historyに追加
		//add_history(command);

		/*
		 *何らかの処理中にコンソールのカーソルが動いてるのはおかしいのでストップしていたカーソル点滅スレッドを再開
		 */
		task_run(ylsh_cursor_timer, -1, 0);

	}

}

/*
 *=======================================================================================
 *シェルの最初の一行を出力する関数
 *=======================================================================================
 */
static void put_first_str(){
	kernel_color_print("[", __RGB256COL__(87, 157, 122));
	kernel_color_print(CurrentDirectory.AbsPath, __RGB256COL__(87, 157, 122));
	kernel_color_print("]", __RGB256COL__(87, 157, 122));
	put_char('\n');
	put_char('%');
	put_char(' ');
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
		if(IS_FAILURE(queue_size(&fifo))){
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
void scroll(int height){
	/*
	 *スクロールに必要なパラメータの取得、計算
	 */
	int i, scrnx = binfo->scrnx, scrny = binfo->scrny;
	int scrnxy = (scrnx * scrny) - (scrnx*height);
	int copy_from = scrnx * height;

	/*
	 *塗替え処理
	 */
	for(i = 0;i < scrnxy;i++){
		binfo->vram[i] = binfo->vram[i+copy_from];
	}

	boxfill8(binfo->vram, scrnx, BLACK, 0, scrny-height, scrnx, scrny);

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
