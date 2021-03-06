[BITS 32]

global write
global exit
global put_char
global open
global sleep
global read
global malloc_init
global malloc
global free
global get_key
global newline
global seek
global stat
global close
global getca

[SECTION .text]

  ;; put_charシステムコール
  ;; 一文字シェルに表示する
  ;; 引数
  ;; int ch...alレジスタ
  ;; =>表示したい文字
put_char:
  mov edx, 1
  mov al, [esp+4]
  int 0x68

  ;; writeシステムコール
  ;; 書き込み系をすべて担当する（させたいなと）システムコール
  ;; 引数
  ;; int fd...eaxレジスタ
  ;; =>ファイルディスクリプタ（ただ言いたかっただけ。）
  ;; char *buf...ebxレジスタ
  ;; =>書き込みデータバッファへのポインタ
  ;; int count...ecxレジスタ
  ;; =>書き込むサイズ
write:
  push  ebx
  mov   edx, 2
  mov   eax, [esp+8]
  mov	  ebx, [esp+12]
  mov   ecx, [esp+16]
  int	  0x68
  pop   ebx
  ret

  ;; readシステムコール
  ;; 読み込み系をすべて担当する（させたいなと）システムコール
  ;; 引数
  ;; int fd...eaxレジスタ
  ;; =>ファイルディスクリプタ（ただ言いたかっただけ。）
  ;; char *buf...ebxレジスタ
  ;; =>読み込みデータバッファへのポインタ
  ;; int count...ecxレジスタ
  ;; =>読み込むサイズ
read:
  push  ebx
  mov   edx, 5
  mov   eax, [esp+8]
  mov   ebx, [esp+12]
  mov   ecx, [esp+16]
  int   0x68
  pop   ebx
  ret

  ;; openシステムコール
  ;; ファイルを開いたり、作成したりするシステムコール
  ;; 引数
  ;; char *pathname...eaxレジスタ
  ;; =>開いたり、作成するファイル名
  ;; int flags...ebxレジスタ
  ;; =>いろいろなフラグ
open:
  push ebx
  mov edx, 3
  mov eax, [esp+8]
  mov ebx, [esp+12]
  int 0x68
  pop ebx
  ret

  ;; mallocの準備用システムコール
  ;; あんまりつかわない
malloc_init:
  push  ebx
  mov   edx, 6
  mov   ebx, [cs:0x0020]  ;malloc領域の開始番地が書いてある
  mov   eax, ebx
  add   eax, 32 * 1024    ;32KB加算する
  mov   ecx, [cs:0x0000]  ;要求するデータセグメントの大きさが書いてある
  sub   ecx, eax          ;
  int   0x68
  pop   ebx
  ret

  ;; mallocシステムコール
  ;; メモリの動的確保システムコール
malloc:
  push  ebx
  mov   edx, 7
  mov   ebx, [cs:0x0020]  ;malloc領域の開始番地が書いてある
  mov   ecx, [esp+8]      ;要求するメモリのサイズ
  int   0x68
  pop   ebx
  ret

free:
  push  ebx
  mov   edx, 8
  mov   ebx, [cs:0x0020]  ;malloc領域の開始番地が書いてある
  mov   eax, [esp+8]      ;解放したいメモリ領域の番地
  mov   ecx, [esp+12]     ;解放したいバイト数
  int   0x68
  pop   ebx
  ret

get_key:
  mov   edx, 9
  mov   eax, [esp+4]  ;キー入力を待つか待たないか
  int   0x68
  ret

  ;; exitシステムコール
  ;; プログラム終了のシステム
  ;; 引数
  ;; 終了状態
exit:
  mov	edx, 4
  mov eax, [esp+4]
  int	0x68

sleep:
  mov   edx, 10                 ;10を代入してdo_sleepを呼び出す
  mov   eax, [esp+4]            ;何ミリ秒待つか
  int   0x68
  ret

  ;; newlineシステムコール
  ;; シェルのインデントとかに使う
  ;; 引数なし
newline:
  mov edx, 11
  int 0x68
  ret

  ;; seekシステムコール
  ;; ファイルのシーク担当のシステムコール
  ;; 引数
  ;; int fd...eaxレジスタ
  ;; =>ファイルディスクリプタ
  ;; off_t offset...ebxレジスタ
  ;; =>オフセット
  ;; int whence...ecxレジスタ
  ;; =>シーク方法
seek:
  push  ebx
	mov   edx, 12
	mov   eax, [esp+8]
	mov   ebx, [esp+12]
	mov   ecx, [esp+16]
	int   0x68
	pop   ebx
	ret

  ;; statシステムコール
  ;; ファイルの情報を取得するシステムコール
  ;; 引数
  ;; int fd...eaxレジスタ
  ;; =>ファイルディスクリプタ
  ;; u32_t *box...ebxレジスタ
  ;; =>データを受け取る箱
stat:
  push ebx
  mov edx, 13
  mov eax, [esp+8]
  mov ebx, [esp+12]
  int 0x68
  pop ebx
  ret

  ;; closeシステムコール
  ;; ファイルを閉じる関数
  ;; ファイルの状態を初期化したりする
  ;; 引数
  ;; int fd...eaxレジスタ
  ;; =>ファイルディスクリプタ
close:
  mov edx, 14
  mov eax, [esp+4]
  int 0x68
  ret

  ;; getcaシステムコール
  ;; コマンドライン引数を取得するシステムコール
  ;; 引数
  ;; int *argc, char **argv
getca:
  push ebx
  mov edx, 15
  mov eax, [esp+8]
  mov ebx, [esp+12]
  int 0x68
  pop ebx
  ret
