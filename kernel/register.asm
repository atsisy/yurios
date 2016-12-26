[BITS 32]

  global load_eip

  [SECTION .text]

;;; eipレジスタの値をロードする関数
load_eip:
  mov eax, [esp]
	ret
