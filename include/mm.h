#ifndef _MM_H
#define _MM_H

#include "kernel.h"

extern unsigned int memtotal;
unsigned int memtest(unsigned int start, unsigned int end);
void memory_init(struct MEMMAN *man);
unsigned int memory_total(struct MEMMAN *man);
unsigned int memory_alloc(struct MEMMAN *man, unsigned int size);
int memory_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
unsigned int memory_alloc_4k(struct MEMMAN *man, unsigned int size);
int memory_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);
char *read_vram(char *vram, short scrnx, int x1, int x2, int y);
void write_vram(char *vram, char *new_vram, short scrnx, int x1, int x2, int y);



/*
 * メモリ管理関数
 */
void *kmalloc(u32_t size);
void kfree(const void *addr, u32_t size);


/****
 * ページング関係
 ****/
/*
 * ページテーブルエントリとか
 */
typedef unsigned long page_table_entry_t;
typedef unsigned long page_directory_entry_t;
typedef unsigned long virtual_address32;
typedef unsigned long virtual_address64;
typedef page_table_entry_t *page_table;
typedef page_directory_entry_t *page_directory;

#define PTE32_PHYSICAL_ADDR 0xfffff000
#define PTE32_PRESENT 0x00000001
#define PTE32_WRITABLE 0x00000002
#define PTE32_US 0x00000004
#define PTE32_PWT 0x00000008
#define PTE32_PCD 0x00000010
#define PTE32_A 0x00000020
#define PTE32_D 0x00000040
#define PTE32_PAT 0x00000080
#define PTE32_G 0x00000100
#define PTE32_AVAILABLE_BITS 0x00000e00

inline u8_t pte32_is_present(page_table_entry_t pte)
{
        return (u32_t)pte & (u32_t)PTE32_PRESENT;
}

inline u8_t pte32_is_writable(page_table_entry_t pte)
{
        return (u32_t)pte & (u32_t)PTE32_WRITABLE;
}

inline void pte32_set_flags(page_table_entry_t *entry, unsigned long flags)
{
        *entry |= flags;
}

inline void pte32_clear_flags(page_table_entry_t *entry, unsigned long flags)
{
        *entry &= ~flags;
}

inline void pte32_set_addr(page_table_entry_t *entry, u32_t addr)
{
        addr &= PTE32_PHYSICAL_ADDR;
        *entry |= addr;
}

inline void *pte32_get_addr(page_table_entry_t entry)
{
        return (void *)((u32_t)entry & (u32_t)PTE32_PHYSICAL_ADDR);
}


#define PDE32_PT_ADDR 0xfffff000
#define PDE32_PRESENT 0x00000001
#define PDE32_WRITABLE 0x00000002
#define PDE32_US 0x0000004
#define PDE32_PWT 0x00000008
#define PDE32_PCD 0x00000010
#define PDE32_A 0x00000020
#define PDE32_RESERVED 0x0000040
#define PDE32_PS 0x0000080
#define PDE32_G 0x0000100
#define PDE_AVAILABLE_BITS 0x00000e00

inline u8_t pde32_is_present(page_directory_entry_t pde)
{
        return (u32_t)pde & (u32_t)PDE32_PRESENT;
}

inline u8_t pde32_is_writable(page_directory_entry_t pde)
{
        return (u32_t)pde & (u32_t)PDE32_WRITABLE;
}

inline void pde32_set_flags(page_directory_entry_t *pde, u32_t flags)
{
        *pde |= flags;
}

inline u8_t pde32_clear_flags(page_directory_entry_t *pde, u32_t flags)
{
        *pde &= ~flags;
}

inline void pde32_set_pt_addr(page_directory_entry_t *pde, u32_t addr)
{
        addr &= PDE32_PT_ADDR;
        *pde |= addr;
}

inline void *pde32_get_pt_addr(page_directory_entry_t pde)
{
        return (void *)((u32_t)pde & (u32_t)PDE32_PT_ADDR);
}

#define MM_PAGE_SIZE 4096
#define KERNEL_PHYSICAL_ADDR 0x00010000
#define VIRTADDR_PTE_MASK 0x000003FF
#define VIRTADDR_PTE_SHIFT 12
#define VIRTADDR_PDE_MASK 0x000003FF
#define VIRTADDR_PDE_SHIFT 22
#define MM_NUM_PTE 1024
#define MM_NUM_PDE 1024
#define MM_PAGE_DIRECTORY_SIZE (sizeof(page_directory_entry) * MM_NUM_PDE)

#define MM_OK 0
#define MM_ERROR (-1)

#define MM_4KIB_ALIGN (0xfffff000)

#define MM_KERNEL_LAND_MEMORY 0x00000000
#define MM_KERNEL_LAND_SIZE   0x40000000
#define MM_USER_LAND_MEMORY   0x40000000
#define MM_USER_LAND_SIZE     0xc0000000

#define VIRT_KERNEL_HEAP 0x30000000

#define MM_1MIB 0x00100000
#define MM_4MIB 0x00400000

#define MM_USER_PT (MM_USER_LAND_MEMORY - (3 * MM_1MIB))
#define MM_KERNEL_PT (MM_USER_PT - (MM_1MIB))
#define MM_PDT (MM_KERNEL_PT - MM_4MIB)

#define MM_KERNEL_HEAP_ADDR (MM_PDT - MM_4MIB)
#define MM_KERNEL_STACK_BASE_ADDR MM_KERNEL_HELP_ADDR

inline unsigned long vaddr32_get_pte_index(virtual_address32 address)
{
        return (address >> VIRTADDR_PTE_SHIFT) & VIRTADDR_PTE_MASK;
}

inline unsigned long vaddr32_get_pde_index(virtual_address32 address)
{
        return (address >> VIRTADDR_PDE_SHIFT) & VIRTADDR_PDE_MASK;
}

inline void pd32_set_pd_address(page_directory dir)
{
        if(dir != NULL){
                store_cr3((unsigned long)dir);
        }
}

void paging_on(void);
u8_t init_virtual_memory_management();
page_table_entry_t *alloc_page32(page_table_entry_t *entry);
void kpage_fault_resolver(virtual_address32 virt_addr);

extern page_directory current_page_directory;

#endif
