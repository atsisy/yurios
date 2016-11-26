/* メモリ関係 */
#include "../include/bootpack.h"
#include "../include/value.h"
#include "../include/sh.h"

unsigned int memtotal;
struct MEMMAN *memman;

unsigned int memtest(unsigned int start, unsigned int end){
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	/* 386か、486以降なのかの確認 */
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT; /* AC-bit = 1 */
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0) { /* 386ではAC=1にしても自動で0に戻ってしまう */
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT; /* AC-bit = 0 */
	io_store_eflags(eflg);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE; /* キャッシュ禁止 */
		store_cr0(cr0);
	}

	i = memtest_sub(start, end);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; /* キャッシュ許可 */
		store_cr0(cr0);
	}

	return i;
}

void memory_init(struct MEMMAN *man){
	man->frees = 0;			//あき情報の個数
	man->maxfrees = 0;		//状況観察用：freesの最大値
	man->lostsize = 0;		//解放に失敗した合計サイズ
	man->losts = 0;			//解放に失敗した回数
	memtotal = memtest(0x00400000, 0xbfffffff);	//メモリの最大容量を計算
	return;
}

/*
 *=======================================================================================
 *memory_total関数
 *メモリの空きサイズの合計を計算する関数
 *=======================================================================================
 */
unsigned int memory_total(struct MEMMAN *man){
	unsigned int i, t = 0;
	for(i = 0; i < memman->frees; i++){
		t += memman->free[i].size;
	}
	return t;
}

/*
 *=======================================================================================
 *memory_alloc関数
 *メモリを確保する関数
 *引数
 *struct MEMMAN *man
 *=>メモリマネージャ構造体
 *unsigned int size
 *確保したいメモリ容量
 *=======================================================================================
 */
unsigned int memory_alloc(struct MEMMAN *man, unsigned int size){
	unsigned int i, a;
	for (i = 0; i < man->frees; i++){
		if (man->free[i].size >= size){
			/* 十分な広さのあきを発見 */
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0) {
				/* free[i]がなくなったので前へつめる */
				man->frees--;
				for (; i < man->frees; i++) {
					man->free[i] = man->free[i + 1]; /* 構造体の代入 */
				}
			}
			return a;
		}
	}

	/*
	 *もう空きが無いため失敗
	 */
	return FAILURE;
}

/*
 *=======================================================================================
 *memory_free関数
 *確保されたメモリを開放する関数
 *引数
 *第一引数
 *=>メモリマネージャ
 *unsigned int addr
 *=>開放するメモリの先頭のアドレス
 *unsigned int size
 *=>開放する合計のメモリ容量
 *=======================================================================================
 */
int memory_free(struct MEMMAN *man, unsigned int addr, unsigned int size){
	int i, j;
	/* まとめやすさを考えると、free[]がaddr順に並んでいるほうがいい
	 	だからまず、どこに入れるべきかを決める */
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].addr > addr) {
			break;
		}
	}
	/* free[i - 1].addr < addr < free[i].addr */
	if (i > 0) {
		/* 前がある */
		if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
			/* 前のあき領域にまとめられる */
			man->free[i - 1].size += size;
			if (i < man->frees) {
				/* 後ろもある */
				if (addr + size == man->free[i].addr) {
					/* なんと後ろともまとめられる */
					man->free[i - 1].size += man->free[i].size;
					/* man->free[i]の削除 */
					/* free[i]がなくなったので前へつめる */
					man->frees--;
					for (; i < man->frees; i++) {
						man->free[i] = man->free[i + 1]; /* 構造体の代入 */
					}
				}
			}
			return 0; /* 成功終了 */
		}
	}
	/* 前とはまとめられなかった */
	if (i < man->frees) {
		/* 後ろがある */
		if (addr + size == man->free[i].addr) {
			/* 後ろとはまとめられる */
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0; /* 成功終了 */
		}
	}
	/* 前にも後ろにもまとめられない */
	if (man->frees < MEMMAN_FREES) {
		/* free[i]より後ろを、後ろへずらして、すきまを作る */
		for (j = man->frees; j > i; j--) {
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees) {
			man->maxfrees = man->frees; /* 最大値を更新 */
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0; /* 成功終了 */
	}
	/* 後ろにずらせなかった */
	man->losts++;
	man->lostsize += size;

	/*
	 *失敗
	 */
	return FAILURE;
}

/*
 *=======================================================================================
 *memory_alloc_4k関数
 *一度に4KB分のメモリを確保する関数
 *詳しい説明はmemory_alloc関数を見ればええよ
 *=======================================================================================
 */
unsigned int memory_alloc_4k(struct MEMMAN *man, unsigned int size){
	unsigned int alloc_memory;
	size = (size + 0xfff) & 0xfffff000;
	alloc_memory = memory_alloc(man, size);
	return alloc_memory;
}

/*
 *=======================================================================================
 *memory_free_4k関数
 *メモリを4KB単位で開放する関数
 *詳しい説明はmemory_free関数を見れえばええよ
 *=======================================================================================
 */
int memory_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size){
	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = memory_free(man, addr, size);
	return i;
}
