#include "../include/elf.h"
#include "../include/string.h"
#include "../include/sh.h"

/*
 *=======================================================================================
 *CheckELF関数
 *この実行ファイルが正式なELFか判定する関数
 *=======================================================================================
 */
u8_t CheckELF(struct Elf32_info *elf){
	return (elf->e_ident[0] == 0x7f && elf->e_ident[1] == 'E' && elf->e_ident[2] == 'L' && elf->e_ident[3] == 'F')
		?
		1 : 0;
}

/*
 *=======================================================================================
 *CutELFSectionHeader関数
 *ELF構造体からセクションヘッダを切り取る関数
 *=======================================================================================
 */
struct Elf32_Shdr *CutELFSectionHeader(struct Elf32_info *elf){
	return (struct Elf32_Shdr *)((int)elf + elf->e_shoff);
}

/*
 *=======================================================================================
 *CutELFSectionHeader関数
 *ELF構造体からプログラムヘッダを切り取る関数
 *=======================================================================================
 */
struct Elf32_Phdr *CutELFProgramHeader(struct Elf32_info *elf){
	return (struct Elf32_Phdr *)((char *)elf + elf->e_phoff);
}

/*
 *=======================================================================================
 *GetELFDataSize関数
 *ELFファイルのデータサイズを得る関数
 *=======================================================================================
 */
u32_t GetELFDataSize(struct Elf32_info *elf){
	int i;
	u32_t limit_addr = 0;
	u32_t dsize = 0;
	struct Elf32_Phdr* phdr = CutELFProgramHeader(elf);

	for(i = 0; i < elf->e_phnum; i++){
		if(!(phdr[i].p_flags & __PF_X__) && limit_addr < phdr[i].p_vaddr){
			limit_addr = phdr[i].p_vaddr;
			dsize = limit_addr + phdr[i].p_memsz;
		}
	}
	return dsize;
}

/*
 *=======================================================================================
 *CopyELFDataSe関数
 *ELFのデータをデータセグメントにコピーする関数
 *=======================================================================================
 */
void CopyELFDataSe(void *DataSegment, struct Elf32_info *elf){
	struct Elf32_Phdr *phdr = CutELFProgramHeader(elf);
	int i;
	for (i = 0; i < elf->e_phnum; i++){
		if(!(phdr[i].p_flags & __PF_X__) && phdr[i].p_filesz > 0){
			/*
			 *データセグメントにコピー
			 */
			memcpy((char *)DataSegment + phdr[i].p_vaddr, (char *)elf + phdr[i].p_offset, phdr[i].p_filesz);
		}
	}
}

/*
 *=======================================================================================
 *FindELFSection関数
 *指定したセクションがELFファイル中に存在するか調べる関数
 *=======================================================================================
 */
struct Elf32_Shdr *FindELFSection(struct Elf32_info *elf, char *SectionName){

	struct Elf32_Shdr *shdr = CutELFSectionHeader(elf);
	char *strtab = elf->e_shstrndx ? (char *)((int)elf + shdr[elf->e_shstrndx].sh_offset) : NULL;
	int i;

	for(i = 0;i < elf->e_shnum;i++){
		if(strcmp(SectionName, strtab + shdr[i].sh_name))  //検証
			return &shdr[i];
	}
	return 0;
}

/*
 *=======================================================================================
 *ELFのスタックポインタの位置を返す関数
 *=======================================================================================
 */
u32_t GetELFEsp(struct Elf32_info *elf){
	/*
	 *スタック用の.stackセクションを探す
	 */
	struct Elf32_Shdr *shdr = FindELFSection(elf, ".stack");
	if(shdr)            //見つかった
		return shdr->sh_size;
	else                //見つからなかった
		return 0;
}

/*
 *=======================================================================================
 *GetAppMM関数
 *アプリケーション用のメモリマネージャを引き出す関数
 *=======================================================================================
 */
struct MEMMAN *GetAppMM(struct Process *proc, struct Elf32_Shdr **p_malloc_SHDR){
	struct Elf32_info *elf = (struct Elf32_info *)proc->cs_val;

	if(!CheckELF(elf))
		return 0;

	struct Elf32_Shdr *m_section = FindELFSection(elf, ".malloc");

	if(p_malloc_SHDR)
		*p_malloc_SHDR = m_section;

	if(!m_section)
		return 0;

	return (struct MEMMAN *)(m_section->sh_addr + proc->ds_val);
}
