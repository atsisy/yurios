/* メモリ関係 */
#include <mm.h>
#include <string.h>
#include "../include/value.h"
#include "../include/sh.h"

unsigned int memtotal;
struct MEMMAN *memman;
page_directory current_page_directory;

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
	return (u32_t)NULL;
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

void *kmalloc(u32_t size)
{
        if(size >= 4096){
                return (void *)memory_alloc_4k(memman, size);
        }else{
                return (void *)memory_alloc(memman, size);
        }
}

void kfree(const void *addr, u32_t size)
{
        if(size >= 4096){
                memory_free_4k(memman, (u32_t)addr, size);
        }else{
                memory_free(memman, (u32_t)addr, size);
        }
}

page_table_entry_t *alloc_page32(page_table_entry_t *entry)
{
        void *physical_address;

        /*
         * 1ページ分確保
         */
        physical_address = memory_alloc_4k(memman, MM_PAGE_SIZE);
        if(physical_address == FAILURE)
                return (page_table_entry_t *)NULL;

        /*
         * アドレスを設定、pフラグを立ててreturn
         */
        pte32_set_addr(entry, (u32_t)physical_address);
        pte32_set_flags(entry, PTE32_PRESENT);
        
        return entry;
}

void free_page32(page_table_entry_t *entry)
{
        void *physical_address;

        physical_address = (void *)pte32_get_addr((u32_t)entry);

        if(physical_address == NULL)
                return;

        memory_free_4k(memman, (u32_t)physical_address, MM_PAGE_SIZE);

        // メモリ上から削除されているため、pフラグを下ろす
        pte32_clear_flags(entry, PTE32_PRESENT);
}


page_table_entry_t *pt32_get_pte(page_table table, virtual_address32 address)
{
        if(table == NULL)
                return (page_table_entry_t *)NULL;

        return (page_table_entry_t *)table + vaddr32_get_pte_index(address);
}

page_directory_entry_t *pd32_get_pde(page_directory dir, virtual_address32 address)
{
        if(dir == NULL)
                return (page_directory_entry_t *)NULL;

        return (page_directory_entry_t *)dir + vaddr32_get_pde_index(address);
}

/*** 
 * map_page関数
 * 物理アドレスと仮想アドレスの対応を取る
 ***/
u8_t map_page(void *physical_address, virtual_address32 virtual_address)
{
        page_directory pd;
        page_directory_entry_t *pde;
        page_table page_table;
        page_table_entry_t *pte;

        pd = current_page_directory;

        // そもそも、ページディレクトリがない場合、エラーで終了
        if(!pd){
                return MM_ERROR;
        }

        // ページディレクトリから、仮想アドレスでpdeを特定
        pde = pd32_get_pde(pd, virtual_address);

        if(!pde32_is_present(*pde)){
                /*
                 * 取得したpdeがメモリ上にない場合、確保し、物理アドレスと対応付け
                 */
                page_table = (page_table_entry_t *)memory_alloc(memman, MM_PAGE_SIZE);
                
                if(page_table)
                        return MM_ERROR;
                memset((void *)page_table, 0x00, MM_PAGE_SIZE);
                pde32_set_pt_addr(pde, (u32_t)page_table);
                pde32_set_flags(pde, PDE32_PRESENT | PDE32_WRITABLE);
        }else{
                /*
                 * ページディレクトリエントリから、ページテーブルを取得
                 */
                page_table = (page_table_entry_t *)pde32_get_pt_addr(pde);
        }

        // 物理アドレスを設定
        pte = pt32_get_pte(page_table, virtual_address);

        pte32_set_flags(pte, PTE32_PRESENT | PTE32_WRITABLE);
        pte32_set_addr(pte, (u32_t)physical_address);

        return MM_OK;
}


static void go_paging32(page_directory directory)
{
        store_cr3((unsigned long)directory);
        paging_on();
}

u8_t init_virtual_memory_management()
{
        //0x00000000~用
        page_table low_table;
        //0xc0000000~用
        page_table high_table;
        
        page_table_entry_t *pte;

        page_directory directory;
        page_directory_entry_t *pde;

        int i;
        unsigned long physical_address;
        virtual_address32 virtual_address;

        /*
         * 0x00000000~
         */
        low_table = (page_table)memory_alloc_4k(memman, MM_PAGE_SIZE);
        if(low_table == FAILURE)
                return MM_ERROR;

        /*
         * 0xc0000000~
         */
        high_table = (page_table)memory_alloc_4k(memman, MM_PAGE_SIZE);
        if(high_table == FAILURE)
                return MM_ERROR;

        // ゼロクリア
        memset((void *)low_table, 0x00, MM_PAGE_SIZE);
        memset((void *)high_table, 0x00, MM_PAGE_SIZE);

        //0x00000000 ~ 0x003ff000
        for(i = 0, physical_address = 0x00, virtual_address = 0x00;
            i < MM_NUM_PTE;
            i++, physical_address += MM_PAGE_SIZE, virtual_address += MM_PAGE_SIZE){
                pte = pt32_get_pte(low_table, virtual_address);
                pte32_set_flags(pte, PTE32_PRESENT | PTE32_WRITABLE);
                pte32_set_addr(pte, physical_address);
        }

        //0xc0000000 ~ 0xc03ff000
        for(i = 0, physical_address = 0x00100000, virtual_address = 0xc0000000;
            i < MM_NUM_PTE;
            i++, physical_address += MM_PAGE_SIZE, virtual_address += MM_PAGE_SIZE){
                pte = pt32_get_pte(high_table, virtual_address);
                pte32_set_flags(pte, PTE32_PRESENT | PTE32_WRITABLE);
                pte32_set_addr(pte, physical_address);
        }

        directory = (page_directory)memory_alloc_4k(memman, MM_PAGE_SIZE);
        if(directory == (void *)FAILURE)
                return MM_ERROR;

        pde = pd32_get_pde(directory, 0x00000000);
        pde32_set_flags(pde, PDE32_PRESENT | PDE32_WRITABLE);
        pde32_set_pt_addr(pde, (u32_t)low_table);

        pde = pd32_get_pde(directory, 0xc0000000);
        pde32_set_flags(pde, PDE32_PRESENT | PDE32_WRITABLE);
        pde32_set_pt_addr(pde, (u32_t)high_table);

        go_paging32(directory);

        return MM_OK;
}
