
BOTPAK	EQU		0x00280000		; bootpackのロード先
DSKCAC	EQU		0x00100000		; ディスクキャッシュの場所
DSKCAC0	EQU		0x00008000		; ディスクキャッシュの場所（リアルモード）

; BOOT_INFO関係
CYLS	EQU		0x0ff0			; ブートセクタが設定する
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; 色数に関する情報。何ビットカラーか？
SCRNX	EQU		0x0ff4			; 解像度のX
SCRNY	EQU		0x0ff6			; 解像度のY
VRAM	EQU		0x0ff8			; グラフィックバッファの開始番地

		org		0xc200			; このプログラムがどこに読み込まれるのか


; キーボードのLED状態をBIOSに教えてもらう

keystatus:
		mov		AH,0x02
		int		0x16 			; keyboard BIOS
		mov		[LEDS],AL

; PICが一切の割り込みを受け付けないようにする
;	AT互換機の仕様では、PICの初期化をするなら、
;	こいつをCLI前にやっておかないと、たまにハングアップする
;	PICの初期化はあとでやる

		mov	al, 0xff
		out	0x21, al
		nop						; OUT命令を連続させるとうまくいかない機種があるらしいので
		;nop命令はCPUを1クロック休憩させる命令
		out	0xa1, al

		cli						; さらにCPUレベルでも割り込み禁止

; CPUから1MB以上のメモリにアクセスできるように、A20GATEを設定

		call	waitkbdout
		mov		AL,0xd1
		OUT		0x64,AL
		call	waitkbdout
		mov		AL,0xdf			; enable A20
		OUT		0x60,AL
		call	waitkbdout

; プロテクトモード移行

		lgdt	[GDTR0]			; 暫定GDTを設定
		mov		EAX,CR0
		and		EAX,0x7fffffff	; bit31を0にする（ページング禁止のため）
		or		EAX,0x00000001	; bit0を1にする（プロテクトモード移行のため）
		mov		CR0,EAX
		jmp		pipelineflush
pipelineflush:
		mov		AX,1*8			;  読み書き可能セグメント32bit
		mov		DS,AX
		mov		ES,AX
		mov		FS,AX
		mov		GS,AX
		mov		SS,AX

; bootpackの転送

		mov		ESI,bootpack	; 転送元
		mov		EDI,BOTPAK		; 転送先
		mov		ECX,512*1024/4
		call	memcpy

; ついでにディスクデータも本来の位置へ転送

; まずはブートセクタから

		mov		ESI,0x7c00		; 転送元
		mov		EDI,DSKCAC		; 転送先
		mov		ECX,512/4
		call	memcpy

; 残り全部

		mov		ESI,DSKCAC0+512	; 転送元
		mov		EDI,DSKCAC+512	; 転送先
		mov		ECX,0
		mov		CL,BYTE [CYLS]
		imul	ECX,512*18*2/4	; シリンダ数からバイト数/4に変換
		sub		ECX,512/4		; IPLの分だけ差し引く
		call	memcpy

; asmheadでしなければいけないことは全部し終わったので、
;	あとはbootpackに任せる

; bootpackの起動

		mov		EBX,BOTPAK
		mov		ECX,[EBX+16]
		add		ECX,3			; ECX += 3;
		shr		ECX,2			; ECX /= 4;
		jz		skip			; 転送するべきものがない
		mov		ESI,[EBX+20]	; 転送元
		add		ESI,EBX
		mov		EDI,[EBX+12]	; 転送先
		call	memcpy
skip:
		mov		ESP,[EBX+12]	; スタック初期値
		jmp		DWORD 2*8:0x0000001b

waitkbdout:
		in		 AL,0x64
		and		 AL,0x02
		in		 AL,0x60 		; から読み(受信バッファが悪さをしないように)
		jnz		waitkbdout		; ANDの結果が0でなければwaitkbdoutへ
		ret

memcpy:
		mov		EAX,[ESI]
		add		ESI,4
		mov		[EDI],EAX
		add		EDI,4
		sub		ECX,1
		jnz		memcpy			; 引き算した結果が0でなければmemcpyへ
		ret
; memcpyはアドレスサイズプリフィクスを入れ忘れなければ、ストリング命令でも書ける

		ALIGN 16, DB 0
GDT0:
		TIMES 8 DB 0				; ヌルセレクタ
		DW		0xffff,0x0000,0x9200,0x00cf	; 読み書き可能セグメント32bit
		DW		0xffff,0x0000,0x9a28,0x0047	; 実行可能セグメント32bit（bootpack用）

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

	ALIGN 16, DB 0
 
bootpack:
