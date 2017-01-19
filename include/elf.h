#ifndef __ELF_H__
#define __ELF_H__

#include "types.h"

#define __ELF_HEADER_BYTES__ 16

/*
 *=======================================================================================
 *ELF解析に使う構造体
 *=======================================================================================
 */
struct ELF32_info {
	/*
	 *ELFの最初の16バイト
	 */
	u8_t header[__ELF_HEADER_BYTES__];

	/*
	 *ファイルのタイプ
	 */
	u16_t type;

	/*
	 *マシンアーキテクチャ
	 */
	u16_t machine;

	/*
	 *ファイルのバージョン
	 */
	u16_t version;

	/*
	 *実行開始アドレス
	 */
	u16_t entry;

	/*
	 *プログラムヘッダテーブルまでのファイル先頭からのオフセット
	 */
	u16_t  phoff;

	/*
	 *セクションヘッダテーブルまでのファイル先頭からのオフセット
	 */
	u16_t  shoff;

	/*
	 *未使用
	 */
	u16_t flags;

	/*
	 *この構造体のサイズ（sizeof(struct elf_info)）
	 */
	u16_t ehsize;

	/*
	 *プログラムヘッダのサイズ
	 */
	u16_t phentsize;

	/*
	 *プログラムヘッダの個数
	 */
	u16_t phnum;

	/*
	 *セクションヘッダのサイズ
	 */
	u16_t shentsize;

	/*
	 *セクションヘッダの個数
	 */
	u16_t shnum;

	/*
	 *セクション名格納用のセクションのセクション番号
	 */
	u16_t shstrndx;
};


struct Elf32_Shdr {
      u32_t sh_name;
	u32_t sh_type;
	u32_t sh_flags;
	u32_t sh_addr;
	u32_t sh_offset;
	u32_t sh_size;
	u32_t sh_link;
	u32_t sh_info;
	u32_t sh_addralign;
	u32_t sh_entsize;
};

struct Elf32_Phdr {
	u32_t p_type;
	u32_t  p_offset;
	u32_t p_vaddr;
	u32_t p_paddr;
	u32_t p_filesz;
	u32_t p_memsz;
	u32_t p_flags;
	u32_t p_align;
};

#endif
