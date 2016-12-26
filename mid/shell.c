#include "../include/kernel.h"
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

	char command[1024];

	struct Process *me = task_now();	//自分自身を指すプロセス構造体

	int shell_buf[128];								//シェルに来るシグナルや割り込み情報をためておくバッファ

	history_init();

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
		ntype_prompt(command, 1024);

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

		strcpy(copied_str, command);

		/*
		 *コマンド打ったあと一個下の行を渡す
		 */
		//indent_shell();

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

			struct i_node inode;
			int fd = do_open("yuri_doc.txt", __O_CREAT__);
			iread(&inode, fd);
			
			char *src = (char *)memory_alloc(memman, 256);
			
			read_mem2hd("cat YURI_DOC.TXT", src, 256);
			
			do_write(fd, src, 1);
			
			memory_free(memman, (u32_t)src, 256);

			fd = do_open("yuri_doc.txt", __O_RDONLY__);
			fadd(fd, "\nSATORI");
			/*
			  do_read(fd, src2, 1);

			  puts(src2);
			*/
		}else if(strcmp(command, "fszeroclear")){
			/*
			  u32_t i;
			  u8_t zero[512] = { 0 };
			  for(i = 0;i < __BLOCKS_LIMIT__;i++)
			  write_ata_sector(&ATA_DEVICE0, i, zero, 1);
			
			  for(i = 0;i < __INODE_LIMIT__;i++)
			  blocks_info[i].exist = __UNUSED_BLOCK__;
			*/
			filesystem_zeroclear();
			goto skip_write_history;
		}else if(strcmp(command, "date")){
			char time[32];
			sprintf(time, "%d:%d %d/%d %d", do_gettime(__HOUR__), do_gettime(__MINUTE__), do_gettime(__MONTH__),
				  do_gettime(__DAY__), do_gettime(__YEAR__));
			puts(time);
		}else if(strcmp(command, "fls")){
			file_list(NULL);
		}else if(strcmp(command, "lscpu")){
			command_lscpu();
		}else if(strcmp(command, "fork")){
			/*
			if(do_fork() == 0){
				puts("child");
			}else{
				puts("parent");
			}
			*/
		}else if(strcmp(command, "ps")){
			extern struct PROCESS_MASTER *process_master;
			u32_t i;
			char p_info[36];
			for(i = 0;i < MAX_TASKS;i++){
				if(process_master->ALL_PROCESSES[i].status == RUNNING_PROCESS){
					zeroclear_8array(p_info, 36);
					sprintf(p_info, "PID:%d %s", process_master->ALL_PROCESSES[i].pid, process_master->ALL_PROCESSES[i].proc_name);
					puts(p_info);
				}
			}
		}else if(strcmp(part, "show")){
			command_show(command);
		}else if(strcmp(command, "yrws")){
			yrsw_main();
		}else if(strcmp(part, "writeyim")){
			char file_name[256];
			cut_string(command, file_name, 9);
			command_writeyim(file_name);
		}else if(strcmp(command, "os")){
			puts("YuriOS Version 0.1.0b\nRelease in x/x, 201x");
		}else if(do_shell_app(fat, copied_str) == 0){
			//対応するコマンドではなく、さらにアプリケーションでもない場合
			/*
			 *外部アプリケーション実行
			 *もしくはそんなコマンドねーよ的な
			 */
			print("Sorry. I Could not recognize '");
			print(copied_str);
			puts("'.");
		}

		/*
		 *次のコマンドを受け付けるための準備
		 */

		/*
		 *historyに追加
		 */
		add_history(command);

	skip_write_history:
		
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

/*
 *外部アプリケーションを実行するための関数
 */
int do_shell_app(int *fat, char *command){
	
	struct FileInfo *finfo;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
	char file_name[13], *p, *q;
	struct Process *me = task_now();
	int i, seg_size, data_size, esp, dathrb;

	zeroclear_8array(file_name, 13);

	/*
	 *ファイル名をコピー
	 */
	for(i = 0;i < 13;i++){
		/*
		 *文字じゃない場合ブレーク
		 *asciiコード表を参照
		 */
		if(command[i] <= ' '){
			break;
		}
		file_name[i] = command[i];
	}

	//最後にNULL文字
	file_name[i] = '\0';
	finfo = file_search(file_name, (struct FileInfo *)(ADR_DISKIMG+0x002600), 224);	//なんかバイナリエディタで確認したら0x2600から始まってた

	/*
	 *拡張子ありの場合
	 */
	if(finfo == 0 && file_name[i-1] != '.'){
		//ただのファイル名では見つからなかったため.yxつきで検索する
		file_name[i] = '.';
		file_name[i+1] = 'y';
		file_name[i+2] = 'x';
		file_name[i+3] = '\0';
		finfo = file_search(file_name, (struct FileInfo *)(ADR_DISKIMG+0x002600), 224);
	}

	/*
	 *ファイルを発見したか？
	 */
	if(finfo != 0){
		/*
		 *ファイルを発見した
		 */

		/*
		 *ファイルをロード
		 */
		p = (char *) memory_alloc_4k(memman, finfo->size);
		loadfile(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));

		/*
		 *ロードしたファイルが正式なものかチェックする
		 */
		if(finfo->size >= 36 && strcmp(p + 4, "yrex") == 1 && *p == 0x00){
                  //ヘッダ解析

			/*
			 *OSが用意するアプリ用のデータセグメントのサイズ
			 */
			seg_size = *((int *)(p + 0x0000));
			/*
			 *espの初期値
			 */
			esp = *((int *)(p + 0x000c));
			/*
			 *データセクションの大きさ
			 */
			data_size = *((int *)(p + 0x0010));
			/*
			 *データ部分の始まり位置
			 */
			dathrb = *((int *)(p + 0x0014));
			
			q = (char *)memory_alloc_4k(memman, seg_size);
			*((int *)0xfe8) = (int)q;
			
			set_segmdesc(gdt + 1003, finfo->size - 1, (int)p, AR_CODE32_ER+0x60);
			set_segmdesc(gdt + 1004, seg_size - 1,    (int)q, AR_DATA32_RW+0x60);

			/*
			 *データ領域をメモリにデータセグメントにコピー
			 */
			for(i = 0; i < data_size; i++){
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
