#include "../include/kernel.h"
#include "../include/value.h"
#include "../include/sh.h"

/*
 *=======================================================================================
 *init_palette関数
 *パレットを初期化する関数
 *=======================================================================================
 */
void init_palette(void){

	/*
	  途中でわからなくなると行けないからとりあえずパレットについてメモ
	  今の画面モードだと8bitカラーが使えるわけだけど、使える256色は勝手に決められてるから
	  こっちから指定しようと言うわけで、プログラマが26番は#012345に指定というようにする仕組みを
	  ”パレット”という
	*/

	static unsigned char table_rgb[16*3] = {
		0x00, 0x00, 0x00,	/*  0:黒 */
		0xff, 0x00, 0x00,	/*  1:明るい赤 */
		0x00, 0xff, 0x00,	/*  2:明るい緑 */
		0xff, 0xff, 0x00,	/*  3:明るい黄色 */
		0x00, 0x00, 0xff,	/*  4:明るい青 */
		0xff, 0x00, 0xff,	/*  5:明るい紫 */
		0x00, 0xff, 0xff,	/*  6:明るい水色 */
		0xff, 0xff, 0xff,	/*  7:白 */
		0xc6, 0xc6, 0xc6,	/*  8:明るい灰色 */
		0x84, 0x00, 0x00,	/*  9:暗い赤 */
		0x00, 0x84, 0x00,	/* 10:暗い緑 */
		0x84, 0x84, 0x00,	/* 11:暗い黄色 */
		0x00, 0x00, 0x84,	/* 12:暗い青 */
		0x84, 0x00, 0x84,	/* 13:暗い紫 */
		0x00, 0x84, 0x84,	/* 14:暗い水色 */
		0x84, 0x84, 0x84	/* 15:暗い灰色 */
	};

	u8_t table256color[216 * 3];
	i32_t r, g, b;

	set_palette(0, 15, table_rgb);

	for(b = 0;b < 6;b++){
		for(g = 0;g < 6;g++){
			for(r = 0;r < 6;r++){
				table256color[(r+g*6+b*36) * 3] = r * 51;
				table256color[(r+g*6+b*36) * 3 + 1] = g * 51;
				table256color[(r+g*6+b*36) * 3 + 2] = b * 51;
			}
		}
	}

	set_palette(16, 233, table256color);

	return;
}

/*
 *=======================================================================================
 *set_palette関数
 *色を設定したりする関数
 *=======================================================================================
 */
void set_palette(int start, int end, unsigned char *rgb){

	/*
	  途中でわからなくなると行けないからとりあえずパレットについてメモ
	  今の画面モードだと8bitカラーが使えるわけだけど、使える256色は勝手に決められてるから
	  こっちから指定しようと言うわけで、プログラマが26番は#012345に指定というようにする仕組みを
	  ”パレット”という
	*/

	int i, eflags;

	eflags = io_load_eflags();	//eflagsレジスタの値を記録しておく
	io_cli();	//割り込み許可フラグを0にして割り込み禁止にする
	io_out8(0x03c8, start);	//パレットを指定するにはまず0x03c8のデバイスに信号を送る必要がある

	for(i = start;i <= end;i++){
		io_out8(0x03c9, rgb[0] >> 2);	//どうやら6bitで値を渡す必要があるらしく、4で割ってるらしい
		io_out8(0x03c9, rgb[1] >> 2);	//動きとしては0x03c9のデバイスにRGBの順に値を送りつけてる
		io_out8(0x03c9, rgb[2] >> 2);
		rgb += 3;	//rgbのメモリ番ＱＺ地を3byte増やす
	}

	io_store_eflags(eflags);	//eflagsレジスタの値を元に戻す
	return;
}

/*
 *=======================================================================================
 *boxfill8関数
 *長方形を描画する関数
 *=======================================================================================
 */
void boxfill8(u8_t *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1){
	int x, y;

	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}

void putfont8(u8_t *vram, int xsize, int x, int y, char c, char *font){

	int i;
	char *p, d /* data */;

	for (i = 0; i < 16; i++) {
		p = (char *)vram + (y + i) * xsize + x;
		d = font[i];

		if ((d & 0x80) != 0)
			p[0] = c;
		if ((d & 0x40) != 0)
			p[1] = c;
		if ((d & 0x20) != 0)
			p[2] = c;
		if ((d & 0x10) != 0)
			p[3] = c;
		if ((d & 0x08) != 0)
			p[4] = c;
		if ((d & 0x04) != 0)
			p[5] = c;
		if ((d & 0x02) != 0)
			p[6] = c;
		if ((d & 0x01) != 0)
			p[7] = c;
	}
	return;
}

/*
 *=======================================================================================
 *putfont8_asc関数
 *asciiコードを渡すと文字を表示する関数
 *引数
 *char *vram
 *ビデオメモリ
 *=======================================================================================
 */
void putfonts8_asc(u8_t *vram, int xsize, int x, int y, char c, char *s){

	extern char dfont[__DFONT_ELEMENTS__];	//フォントデータ

	for (;*s != 0x00;s++){
		/*
		*改行文字がきたら改行する
		*/
		if(*s == 0x0a){
			x = 0;
			y  += 16;
			indent_shell();
			continue;
		}

		/*
		*普通に文字を表示
		*/
		putfont8(vram, xsize, x, y, c, dfont + *s * 16);
		x += 8;

	}
	return;
}

/*
 *=======================================================================================
 *draw_vline関数
 *画面上に縦線を描画する関数
 *=======================================================================================
 */
void draw_vline(unsigned char *vram, int screen_x, int screen_y, int draw_x, int draw_y){

	int x, y;
	x = 0;
	for(y = 0;y < draw_y;y++){
		x += draw_x;
		vram[x] = COL8_FFFFFF;
		x += (screen_x-draw_x);
	}
	putfonts8_asc(vram, screen_x, binfo->scrnx-140, 20, COL8_FFFFFF, "Running Processes");
}

/*
 *=======================================================================================
 *draw_hline関数
 *画面上に横戦を描画する関数
 *=======================================================================================
 */
void draw_hline(unsigned char *vram, int screen_x, int screen_y, int draw_x, int draw_y){

	int x;
	for(x = 0;x < draw_x;x++){
		vram[screen_x*draw_y+x] = COL8_FFFFFF;
	}
}
