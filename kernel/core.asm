
[BITS 32]


;自由に使っていい32bitレジスタeax, ecx, edx	 32ビットモード用の機械語を作らせる
; オブジェクトファイルのための情報

		; このプログラムに含まれる関数名
global	io_hlt, io_cli, io_sti, io_stihlt
global	io_in8, io_in16, io_in32
global	io_out8, io_out16, io_out32
global	io_load_eflags, io_store_eflags
global	load_gdtr, load_idtr
  global	load_cr0, store_cr0
  global  load_cr3, store_cr3
  global flush_tlb, paging_on
global	asm_inthandler21
global	asm_inthandler20, asm_inthandler0d
global	asm_inthandler0c, asm_inthandler00
global	memtest_sub
global	farjmp, run_app
global	farcall, start_app
global	load_tr
global	asm_put_char
  global	asm_sys_call
  global print_a
extern	keyboard_handler
extern 	mouse_handler
extern	timer_handler
extern	stack_exp_handler
extern	sys_call
extern	general_exp_handler
extern	zzdiv_handler  

; 以下は実際の関数

[SECTION .text]		; オブジェクトファイルではこれを書いてからプログラムを書くらしい

  ;; hlt命令
  ;; 割り込みが来るまで待機
io_hlt:	; void io_hlt(void);
	hlt
	ret

  ;; cli命令
  ;; 割り込み禁止にする
io_cli:	; void io_cli(void);で使う
	cli
	ret

  ;; sti命令
  ;; 割り込みを許可する
io_sti:
	sti
	ret

print_a:
  	mov word [0x000B8000], 0x4128

  ;; sti命令とhlt命令を同時にするアセンブリ関数
  ;; つまり、割り込み許可してから、黙り込む
io_stihlt:
	sti
	hlt
	ret

  ;; 8bit インプット命令
io_in8:	; int io_in8(int port);で使う
	mov	edx, [esp+4]	;引数portをedxレジスタに代入
	mov	eax, 0
	in	al, dx
	ret

  ;; 16bit インプット命令
io_in16:	; int io_in16(int port);で使う
	mov	edx, [esp+4]	;引数portをedxレジスタに代入
	mov	eax, 0
	in	ax, dx
	ret

  ;; 32bit インプット命令
io_in32:	; int io_in32(int port);で使う
	mov	edx, [esp+4]	;引数portをedxレジスタに代入
	in	eax, dx
	ret

  ;; 8bit アウトプット命令
io_out8:	; void io_out8(int port, int data);で使う
	mov	edx, [esp+4]	;引数portをedxレジスタに代入
	mov	al, [esp+8]		;引数dataをalレジスタに代入
	out	dx, al
	ret

  ;; 16bit アウトプット命令
io_out16:	; void io_out16(int port, int data);で使う
	mov	edx, [esp+4]	;引数portをedxレジスタに代入
	mov	eax, [esp+8]		;引数dataをalレジスタに代入
	out	dx, ax
	ret

  ;; 32bit アウトプット命令
io_out32:	; void io_out32(int port, int data);で使う
	mov	edx, [esp+4]	;引数portをedxレジスタに代入
	mov	eax, [esp+8]		;引数dataをalレジスタに代入
	out	dx, eax
	ret

  ;; eflagsレジスタを見る関数
io_load_eflags:	;int io_load_eflags(void);で使う
	pushfd	;push eflagsという意味らしい。スタック_asm_hrb_api:
	pop	eax	;スタックからpopして、eaxに代入
	ret

  ;; eflagsレジスタにぶち込む
io_store_eflags:	;void io_store_eflags(int eflags);で使う
	mov	eax, [esp+4]	;引数eflagsをeaxレジスタに代入
	push	eax	;スタックにeaxの値を積む
	popfd	;スタックからpopして、eflagsに代入
	ret

load_gdtr:
	mov	ax, [esp+4]
	mov [esp+6], ax
	lgdt	[esp+6]
	ret

load_idtr:
	mov ax, [esp+4]
	mov [esp+6], ax
	lidt	[esp+6]
	ret

  ;; cr0を見るアセンブリ関数
load_cr0:		; int load_cr0(void);
	mov		eax, cr0
	ret

load_cr3:
  mov eax, cr3
  ret

  ;; cr0に値をぶち込むアセンブリ関数
store_cr0:
	mov	eax, [esp+4]
	mov	cr0, eax
	ret

store_cr3:
  mov eax, [esp+4]
  mov cr3, eax
  ret

flush_tlb:
  push eax
  mov eax, [esp+4]
  cli
  invlpg [esp+4]
  sti
  pop eax
  ret

paging_on:
  cli
  push eax
  mov eax, cr0
  or eax, 0x80000000
  mov cr0, eax                  
  pop eax
  sti
  ret

  ;; ゼロ除算例外のイベントハンドラ
asm_inthandler00:
	sti
	push es
	push ds
	pushad
	mov  eax, esp
	push eax
	mov  ax, ss
	mov  ds, ax
	mov  es, ax
	call zzdiv_handler
	cmp  eax, 0
	jne  end_app
	pop  eax
	popad
	pop	 ds
	pop  es
	add  esp, 4
	iretd

  ;; スタック例外のイベントハンドラ
asm_inthandler0c:
	sti
	push es
	push ds
	pushad
	mov  eax, esp
	push eax
	mov  ax, ss
	mov  ds, ax
	mov  es, ax
	call stack_exp_handler
	cmp  eax, 0
	jne  end_app
	pop  eax
	popad
	pop	 ds
	pop  es
	add  esp, 4
	iretd

  ;; 一般保護例外のイベントハンドラ
asm_inthandler0d:
	sti
	push	es
	push	ds
	pushad
	mov		eax, esp
	push	eax
	mov		ax, ss
	mov		ds, ax
	mov		es, ax
	call	general_exp_handler
	cmp		eax, 0		; ここだけ違う
	jne		end_app		; ここだけ違う
	pop		eax
	popad
	pop		ds
	pop		es
	add		esp, 4			; INT 0x0d では、これが必要
	iretd


  ;; タイマ割り込みイベントハンドラ
asm_inthandler20:
	push	es
	push	ds
	pushad
	mov	eax, esp
	push	eax
	mov	ax, ss
	mov	ds, ax
	mov	es, ax
	call	timer_handler
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

  ;; キーボード割り込みイベントハンドラ
asm_inthandler21:
	push	es
	push	ds
	pushad
	mov	eax, esp
	push	eax
	mov	ax, ss
	mov	ds, ax
	mov	es, ax
	call	keyboard_handler
	pop	eax
	popad
	pop	ds
	pop	es
	iretd
  
memtest_sub:	; unsigned int memtest_sub(unsigned int start, unsigned int end)
	push	edi
	push	esi
	push	ebx
	mov		esi, 0xaa55aa55			; pat0 = 0xaa55aa55;
	mov		edi, 0x55aa55aa			; pat1 = 0x55aa55aa;
	mov		eax, [esp+12+4]			; i = start;
mts_loop:
	mov		ebx, eax
	add		ebx, 0xffc				; p = i + 0xffc;
	mov		edx, [ebx]				; old = *p;
	mov		[ebx], esi				; *p = pat0;
	xor		dword [ebx], 0xffffffff	; *p ^= 0xffffffff;
	cmp		edi, [ebx]				; if (*p != pat1) goto fin;
	jne		mts_fin
	xor		dword [ebx], 0xffffffff	; *p ^= 0xffffffff;
	cmp		esi, [ebx]				; if (*p != pat0) goto fin;
	jne		mts_fin
	mov		[ebx], edx				; *p = old;
	add		eax, 0x1000				; i += 0x1000;
	cmp		eax, [esp+12+8]			; if (i <= end) goto mts_loop;
	jbe		mts_loop
	pop		ebx
	pop		esi
	pop		edi
	ret
mts_fin:
	mov		[ebx], edx				; *p = old;
	pop		ebx
	pop		esi
	pop		edi
	ret

load_tr:	;void load_tr(int tr);
	ltr	[esp+4]	;trに第一引数を代入
	ret

farjmp:	;void farjmp(int eip, int cs);
	jmp	far	[esp+4]
	ret

farcall:		; void farcall(int eip, int cs);
		call	far	[esp+4]				; eip, cs
		ret

  ;; システムコール受付のアセンブリ関数
asm_sys_call:
		sti
		push	ds
		push	es
		pushad					;保存のためのPUSH
		pushad					;sys_callにわたすためのPUSH
		mov		ax, ss
		mov		ds, ax		; OS用のセグメントをDSとESにも入れる
		mov		es, ax
		call	sys_call
		cmp		eax, 0		; EAXが0でなければアプリ終了処理
		jne		end_app
		add		esp, 32
		popad
		pop		es
		pop		ds
		iretd
end_app:
;	eaxはtss.esp0の番地
		mov		esp, [eax]
		popad
		ret					; cmd_appへ帰る

  ;; アプリケーションを開始する
start_app:		; void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
		pushad		; 32ビットレジスタを全部保存しておく
		mov		eax, [esp+36]	; アプリ用のEIP
		mov		ecx, [esp+40]	; アプリ用のCS
		mov		edx, [esp+44]	; アプリ用のESP
		mov		ebx, [esp+48]	; アプリ用のDS/SS
		mov		ebp, [esp+52]	; tss.esp0の番地
		mov		[ebp], esp		; OS用のESPを保存
		mov		[ebp+4], ss		; OS用のSSを保存
		mov		es, bx
		mov		ds, bx
		mov		fs, bx
		mov		gs, bx
;	以下はRETFでアプリに行かせるためのスタック調整
		or		ecx, 3			; アプリ用のセグメント番号に3をORする
		or		ebx, 3			; アプリ用のセグメント番号に3をORする
		push	ebx				; アプリのSS
		push	edx				; アプリのESP
		push	ecx				; アプリのCS
		push	eax				; アプリのEIP
		retf
		;	アプリが終了してもここには来ない
