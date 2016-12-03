[BITS 32]

;;; CPUの情報とかを調べるアセンブリ関数群

global cpu_vendor

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

