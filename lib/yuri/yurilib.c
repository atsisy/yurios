#include "../../include/kernel.h"
#include "../../include/sh.h"
#include "../../include/elf.h"
#include "../../include/string.h"
#include "../../include/sysc.h"
#include "../../include/yrfs.h"

void readfat(int *fat, unsigned char *img);
void loadfat(int clustno, int size, char *buf, int *fat, char *img);

void readfat(int *fat, unsigned char *img){	//FATの圧縮を解く
	int i, j = 0;
	for(i = 0;i < 2880;i+=2){
		fat[i + 0] = (img[j+0] | img[j+1] << 8) & 0xfff;
		fat[i + 1] = (img[j+1] >> 4 | img[j+4] << 4) & 0xfff;
		j += 3;
	}
	return;
}

void loadfile(int clustno, int size, char *buf, int *fat, char *img){
	int i;
	for(;;){
		if(size <= 512){
			for(i = 0;i < size;i++){
				buf[i] = img[clustno * 512 + i];
			}
			break;
		}
		for(i = 0;i < 512;i++){
			buf[i] = img[clustno * 512 + i];
		}
		size -= 512;
		buf += 512;
		clustno = fat[clustno];
	}

	return;
}

int sprintf(char *str, const char *format, ...){
	int *argv = (int *)(&str+2);	//可変個引数の配列
	int count, i, argc = 0;			//生成した文字数
	char buf[40];		     //変換した文字列を格納するバッファ
	const char *p = format;      //コピー元フォーマットを走査する

	/*
	 *バッファをゼロクリア
	 */
	zero_str(str);
	zero_str(buf);

	/*
	 *ヌル文字になるまで
	 */
	for(count = 0; *p != '\0'; p++) {
		switch(*p) {
		case '%':
			//フォーマット指定子かどうか判定し、引数の数値を文字列へ変換
			switch(p[1]) {
			case 'd':    //10進数
				int2dec(buf, argv[argc++]);
				break;
			case 'c':    //文字
				int2char(buf, argv[argc++]);
				break;
			case 'x':    //16進数 小文字
				int2hex(buf, argv[argc++]);
				break;
			case 's':    //文字列
				int2str(buf, argv[argc++]);
				break;
			}

			/*
			 *変換結果をコピー
			 */
			for(i = 0;buf[i] != '\0';i++,count++) {
				*str++ = buf[i];
			}
			p++; //一個前にすすめる
			break;

		default:
			// フォーマット指定子以外はそのままコピー
			*str++ = *p;
			count++;
		}
	}

	str[count] = '\0';

	return count;
}

/*
 *=======================================================================================
 *zero_str関数
 *文字列を\0で埋める関数
 *=======================================================================================
 */
void zero_str(char *str){
	while(*str != '\0')
		*str++ = 0;
}

/*
 *=======================================================================================
 *zeroclear_8array関数
 *配列をゼロクリアする関数
 *=======================================================================================
 */
void zeroclear_8array(i8_t *array, u32_t length){
	u32_t i;
	for(i = 0;i < length;i++)
		array[i] = 0;
}

/*
 *=======================================================================================
 *zeroclear_short_array関数
 *配列をゼロクリアする関数
 *=======================================================================================
 */
void zeroclear_16array(i16_t *array, u32_t length){
	u32_t i;
	for(i = 0;i < length;i++)
		array[i] = 0;
}

/*
 *=======================================================================================
 *zeroclear_short_array関数
 *配列をゼロクリアする関数
 *=======================================================================================
 */
void zeroclear_32array(i32_t *array, u32_t length){
	u32_t i;
	for(i = 0;i < length;i++)
		array[i] = 0;
}
/*
 *=======================================================================================
 *int2char関数
 *文字コードを文字に変換する
 *=======================================================================================
 */
void int2char(char *str, int value){
	str[0] = (char)value;
	str[1] = '\0';
}

//数値を16進数文字列に変換する
//flag: 大文字なら1, 小文字なら0
void int2hex(char *str, int value) {

	int i, n, zero = 0, mask = 0x0f;
	char charctor = 'a';

	for(i = 0; i < 8; i++) {
		n = value >> (7-i)*4;

		if(!zero && n != 0){
			/*
			 *0以外が初めて来た
			 */
			zero = 1;
		}else if(!zero){
			/*
			 *0ではない
			 */
			continue;
		}

		if((n & mask) >= 10) {
			/*
			 *文字に変換
			 */
			*str++ = charctor + (n & mask)-10;
		}else{
			/*
			 *数字に変換
			 */
			*str++ = '0' + (n & mask);
		}
	}

	/*
	 *最後にヌル文字
	 */
	*str = '\0';
}

/*
 *=======================================================================================
 *10進数の指定された桁を返す関数
 *=======================================================================================
 */
int dec_digit(int value, int n){
	int i;
	for(i = 0; i < n-1; i++){
		value /= 10;
	}
	return value % 10;
}

/*
 *=======================================================================================
 *int2dec関数
 *普通に10進数を文字列にする関数
 *=======================================================================================
 */
void int2dec(char *str, int value) {
	int i;
	char zero = 1;

	if(!value){
		str[0] = '0';
		str[1] = '\0';
		return;
	}

	if(value < 0){
		str[0] = '-';
		str += 1;
		value = -value;
	}

	for(i = 0;i < 10;i++){
		if(zero && dec_digit(value, 10-i) != 0)
			zero = 0;
		if(!zero)
			*str++ = '0' + dec_digit(value, 10-i);
	}

	*str = '\0';
}

/*
 *=======================================================================================
 *int2str関数
 *普通に数字を文字列にする関数
 *=======================================================================================
 */
void int2str(char *str, int value) {
	char *p = (char *)value;
	strcpy(str, p);
}


int *sys_call(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax) {

	struct Process *me = task_now();

	int i;

	int cs_base = me->cs_val;
	int ds_base = me->ds_val;

	/*
	  アセンブリの方で２回pushadをしてレジスタをバックアップしている
	  レジスタの順番的にeaxが橋になているから、その次は二回目のバックアップ
	  したレジスタの値がまっている。その値を書き換えてアプリケーションに
	  返り値を渡す
	*/
	int *registers = &eax + 1;	//eaxの次の番地

	/*
	  メモ
	  *registersについて
	  インデックスが。。。
	  0:edi 1:esi 2:ebp 3:esp 4:ebx 5:edx
	  6:ecx 7:eax
	*/

	switch(edx){
	case 1:
		/*
		 *=======================================================================================
		 *put_charシステムコール
		 *=======================================================================================
		 */
		put_char(eax & 0xff);
		break;
	case 2:
		/*
		 *=======================================================================================
		 *writeシステムコール
		 *=======================================================================================
		 */

		//eaxに返り値を格納
		registers[7] = do_write(eax, (char *)(ebx + ds_base), ecx);
		break;
	case 3:
	      /*
		 *=======================================================================================
		 *openシステムコール
		 *=======================================================================================
		 */
		registers[7] = do_open((char *)(eax + ds_base), ebx);
		break;
	case 4:
		/*
		 *=======================================================================================
		 *exitシステムコール
		 *=======================================================================================
		 */
		if(!eax){
			/*
			 *正常終了
			 */
			return &(me->tss.esp0);
		}else{
			pError("A non-zero value was returned by application.");
			return &(me->tss.esp0);
		}
		
	case 5:
		/*
		 *=======================================================================================
		 *readシステムコール
		 *=======================================================================================
		 */
		registers[7] = do_read(eax, (char *)(ebx + ds_base), ecx);
		break;
	case 6:
	{
	      /*
		 *=======================================================================================
		 *メモリマネージャ初期化のシステムコール
		 *ebx:メモリマネージャの番地
		 *eax:メモリマネージャが管理する最初の番地
		 *ecx:メモリマネージャが管理する領域のサイズ
		 *=======================================================================================
		 */
		/*
		memory_init((struct MEMMAN *)(ebx + cs_base));
		ecx &= 0xfffffff0;	//16バイト単位にする
		memory_free((struct MEMMAN *)(ebx + cs_base), eax, ecx);
		*/

		struct Elf32_info *elf = (struct Elf32_info *)cs_base;

		//このアプリケーションはELFか？
		if(CheckELF(elf)){
			struct Elf32_Shdr *m_section;
			memman = GetAppMM(me, &m_section);

			if(!memman)
				return &(me->tss.esp0);

			memory_init(memman);
			u32_t data_off = m_section->sh_addr + 32 * 1024;
			u32_t data_size = GetELFDataSize(elf) - data_off;
			//16バイトごとになるようにマスク
			data_size &= 0xfffffff0;
			memory_free(memman, data_off, data_size);
		}else{
			memory_init((struct MEMMAN *)(ebx + ds_base));
			ecx &= 0xfffffff0;
			memory_free((struct MEMMAN *)(ebx + ds_base), eax, ecx);
		}
		break;
	}
	case 7:
	      /*
		 *=======================================================================================
		 *mallocもどきシステムコール
		 *ebx:メモリマネージャの番地
		 *ecx:要求するメモリ容量
		 *eax:実際に確保したメモリ番地
		 *=======================================================================================
		 */
		ecx = (ecx+0x0f) & 0xfffffff0;	//16バイト単位にする

		if(CheckELF((struct Elf32_info *)cs_base))
			memman = GetAppMM(me, 0);
		else
			memman = (struct MEMMAN *)(ebx + ds_base);

		//結果を返す
		registers[7] = memory_alloc(memman, ecx);
		break;
	case 8:
	      /*
		 *=======================================================================================
		 *freeもどきシステムコール
		 *ebx:メモリマネージャの番地
		 *eax:解放したいメモリ領域の番地
		 *ecx:解放したいバイト数
		 *=======================================================================================
		 */
		ecx = (ecx+0x0f) & 0xfffffff0;	//16バイト単位にする
		if(CheckELF((struct Elf32_info *)cs_base))
			memman = GetAppMM(me, 0);
		else
			memman = (struct MEMMAN *)(ebx + ds_base);

		memory_free(memman, eax, ecx);
		break;
	case 9:
	      /*
		 *=======================================================================================
		 *キー入力に関するシステムコール
		 *キー入力があればそれを返す
		 *eax:キー入力を待つか待たないか
		 *0...待たない 1...待つ
		 *=======================================================================================
		 */
		//struct FIFO32 *before = now_keybuf();
		//ch_keybuf(&(me->irq));

		for(;;){
			io_cli();
			if(queue_size(me->irq) == 0){
				if(eax != 0){
					task_sleep(me);	/* FIFOが空なので寝て待つ */
				}else{
					io_sti();
					registers[7] = -1;
					return 0;
				}
			}else{
				i = queue_pop(me->irq);
				io_sti();
				if('A' <= keys1[i-256] && 'Z' >= keys1[0-511]){ /* キーボードデータ（タスクA経由） */
					registers[7] = keys1[i - 256];
					return 0;
				}
			}
		}
		break;
	case 10:
	      /*
		 *=======================================================================================
		 *sleepシステムコール
		 *eax:スリープする時間(ミリ秒)
		 *=======================================================================================
		 */
		do_sleep((unsigned int)eax);
		break;
	case 11:
		/*
		 *=======================================================================================
		 *newlineシステムコール
		 *=======================================================================================
		 */
		indent_shell();
		break;
      case 12:
		/*
		 *=======================================================================================
		 *seekシステムコール
		 *=======================================================================================
		 */
		registers[7] = do_seek(eax, ebx, ecx);
		break;
	case 13:
		/*
		 *=======================================================================================
		 *statシステムコール
		 *=======================================================================================
		 */
		registers[7] = do_stat(eax, ((u32_t *)((char *)ebx + cs_base)));
		break;
	case 14:
		/*
		 *=======================================================================================
		 *closeシステムコール
		 *=======================================================================================
		 */
		registers[7] = do_close(eax);
		break;
	case 15:
	{
		u32_t size;
		/*
		 *=======================================================================================
		 *getcaシステムコール
		 *=======================================================================================
		 */
		 //me->argcが変な値だったら-1を返す
		registers[7] = *((int *)eax) = me->argc > 0 ? me->argc : -1;

		strcpy((char *)(ebx + ds_base), me->cmd_line);
		puts((char *)(ebx + ds_base));

		break;
	}
	}


	return 0;
}

/*
 *=======================================================================================
 *osAtoi関数
 *atoi関数もどき
 *=======================================================================================
 */
i32_t osAtoi(char *str){
	i32_t num = 0;

	while(*str){
		//文字コード分引く
		num += *str - 48;
		num *= 10;
		str++;
	}
	num /= 10;

	return num;
}
