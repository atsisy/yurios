[BITS 32]

;;; CPUの情報とかを調べるアセンブリ関数群

global cpu_vendor
global cpu_geneinfo_sub
global cpu_cache_sub

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
