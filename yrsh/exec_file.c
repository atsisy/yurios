#include "../include/kernel.h"
#include "../include/sh.h"
#include "../include/elf.h"
#include "../include/string.h"

u8_t *read_elf(char *file_name, u8_t *buffer, u32_t length);

/*
 *=======================================================================================
 *do_shell_app関数
 *外部アプリケーションを実行するための関数
 *=======================================================================================
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

			//*((char ***)(esp + 8)) = argv;
			*((u32_t *)(esp + 4)) = 875;
			
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

/*
 *=======================================================================================
 *start_elf_app関数
 *新しいELF形式の実行ファイルを実行する関数
 *引数
 *struct Process *proc
 *今のプロセス
 *void *text
 *メモリにマップした機械語の開始位置
 *int text_size
 *機械語のサイズ
 *void *data
 *メモリにマップされたデータ領域の開始位置
 *int data_size
 *データ領域のサイズ
 *int eip
 *eip
 *int cs
 *cs
 *int esp
 *esp
 *int ds
 *ds
 *=======================================================================================
 */
void start_elf_app(struct Process *proc, void *text, int text_size, void *data, int data_size, int eip, int cs, int esp, int ds){
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
	
	proc->cs_val = (int)text;
	proc->ds_val = (int)data;

	set_segmdesc(gdt + 1003, text_size - 1, (int) text, AR_CODE32_ER + 0x60);
	set_segmdesc(gdt + 1004, data_size - 1, (int) data, AR_DATA32_RW + 0x60);

	start_app(eip, 1003 * 8, esp, 1004 * 8, &(proc->tss.esp0));
}

/*
 *=======================================================================================
 *do_shell_app関数
 *外部アプリケーションを実行するための関数
 *=======================================================================================
 */
int exec_elf_app(int *fat, char *command){

	struct FileInfo *finfo;
	char file_name[13], *p, *q;
	struct Process *me = task_now();
	int i, seg_size, esp, app_size;

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

		//コピー
		file_name[i] = command[i];
	}

	//最後にNULL文字
	file_name[i] = '\0';

	//なんかバイナリエディタで確認したら0x2600から始まってた
	finfo = file_search(file_name, (struct FileInfo *)(ADR_DISKIMG+0x002600), 224);

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

		//アプリケーションのサイズをコピー
		app_size = finfo->size;

		/*
		 *ファイルをロード
		 */
		p = (char *)memory_alloc_4k(memman, app_size);

		//ELF用のローダ
		read_elf(file_name, (u8_t *)p, app_size);

		/*
		 *ロードしたファイルが正式なものかチェックする
		 */
	      if(app_size >= (i32_t)sizeof(struct Elf32_info) && CheckELF((struct Elf32_info *)p)){
			struct Elf32_info *elf = (struct Elf32_info *)p;

			if(!(esp = GetELFEsp(elf)))   //エラーで終了
				goto start_app_end;

			seg_size = GetELFDataSize(elf);
			q = (char *)memory_alloc_4k(memman, seg_size);
			CopyELFDataSe(q, elf);
			start_elf_app(me, p, app_size, q, seg_size, elf->e_entry, 0 * 8 + 4, esp, 1 * 8 + 4);
			memory_free_4k(memman, (u32_t)q, seg_size);

		}else{
			pError("Yuri executable file format error.");
		}

	start_app_end:

		memory_free_4k(memman, (u32_t)p, finfo->size);

		return 1;
	}
	//ファイルが見つからなかった場合
	return 0;
}
