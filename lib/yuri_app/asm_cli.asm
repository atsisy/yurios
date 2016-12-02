[BITS 32]

global write, exit
global put_char
global open
global sleep
global read
global malloc_init, malloc
global free, get_key

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

exit:
  mov		edx, 4
  int		0x68

sleep:
  mov   edx, 10                 ;10を代入してdo_sleepを呼び出す
  mov   eax, [esp+4]            ;何ミリ秒待つか
  int   0x68
  ret
