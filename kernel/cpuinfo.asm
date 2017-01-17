[BITS 32]

;;; CPUの情報とかを調べるアセンブリ関数群

global cpu_vendor
global cpu_geneinfo_sub
global cpu_cache_sub
global cpu_brand_string

;;; CPUベンダと拡張性を調べるアセンブリ関数
;;; void cpu_vendor(u32_t *ebx, u32_t *ecx, u32_t *edx);
cpu_vendor:
  ;; バックアップ
  push ebx
  push ecx
  push edx
  push esi
  ;; CPUベンダと拡張性を調べるため0x00を代入
  mov eax, 0x00
  ;; CPUIDを実行
  cpuid
  ;; eaxをバックアップ
  mov esi, eax
  ;; 引数のポインタに結果を代入
  mov eax, [esp+20]
  mov [eax], ebx
 	mov eax, [esp+24]
  mov [eax], ecx
  mov eax, [esp+28]
  mov [eax], edx
  ;; 元に戻す
  mov eax, esi
  pop esi
  pop edx
  pop ecx
  pop ebx
  ret

  ;;; CPUの基本情報を調べるアセンブリ関数
  ;;; void cpu_geneinfo_sub(u32_t *ebx, u32_t *ecx, u32_t *edx);
cpu_geneinfo_sub:
      ;;バックアップ
      push ebx
      push ecx
      push edx
      push esi
      ;;CPUの基本的な情報を得るためにwaxに0x01を代入
      mov eax, 0x01
      ;;CPUIDを実行
      cpuid
      mov esi, eax
      ;; 引数のポインタに結果を代入
      mov eax, [esp+20]
      mov [eax], ebx
     	mov eax, [esp+24]
      mov [eax], ecx
      mov eax, [esp+28]
      mov [eax], edx
      ;; 元に戻す
      mov eax, esi
      pop esi
      pop edx
      pop ecx
      pop ebx
      ret


;;; CPUのキャッシュ情報を得る関数
;;; void cpu_cache_sub(u32_t *ebx, u32_t *ecx, u32_t *edx);
cpu_cache_sub:
    ;;バックアップ
    push ebx
    push ecx
    push edx
    push esi
    ;;CPUの基本的な情報を得るためにwaxに0x01を代入
    mov eax, 0x02
    ;;CPUIDを実行
    cpuid
    mov esi, eax
    ;; 引数のポインタに結果を代入
    mov eax, [esp+20]
    mov [eax], ebx
    mov eax, [esp+24]
    mov [eax], ecx
    mov eax, [esp+28]
    mov [eax], edx
    ;; 元に戻す
    mov eax, esi
    pop esi
    pop edx
    pop ecx
    pop ebx
    ret

;;; CPUのブランドストリングを取得する関数
;;; i32_t cpu_brand_string(char *string);
cpu_brand_string:
    	;;バックアップ
	push ebx
	push ecx
	push edx
	push esi
  ;; CPUIDの拡張命令に対応しているか判定
  mov eax, 0x01
  ;; CPUIDを実行
  cpuid
  and eax, 0x80000000
  jz not_support
  ;; 以下ブランドストリング取得の拡張CPUID命令に対応している場合
  ;; CPUID実行後、EAXレジスタにCPUID拡張機能の最大インデックスがセットされる

  ;; プロセッサブランドストリングに対応しているかどうか
  sub eax, 0x80000004
  jz not_support
  ;; プロセッサブランドストリングに対応している
  mov eax, 0x80000002
  cpuid
  mov esi, eax
  mov eax, [esp+20]
  mov [eax], esi
  mov [eax+4], ebx
  mov [eax+8], ecx
  mov [eax+12], edx
  mov eax, 1
  jmp cpuid_end

not_support:
  ;; ブランドストリング取得の拡張CPUID命令に対応していない
  mov eax, -1
  jmp cpuid_end

cpuid_end:
  pop esi
	pop edx
	pop ecx
	pop ebx
	ret
