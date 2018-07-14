#include "../../include/mm.h"
#include "../../include/sh.h"
#include "../../include/elf.h"
#include "../../include/string.h"
#include "../../include/sysc.h"
#include "../../include/yrfs.h"
#include "../../include/util_macro.h"

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
			puts("A non-zero value was returned by application.");
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
		registers[7] = kmalloc(ecx);
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
                /*
                 * FIXME
                 */
		//indent_shell();
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
		/*
		 *=======================================================================================
		 *getcaシステムコール
		 *=======================================================================================
		 */
		 //me->argcが変な値だったら-1を返す
		registers[7] = *((int *)(eax + cs_base)) = me->argc > 0 ? me->argc : -1;

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
