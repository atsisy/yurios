#include <sh.h>
#include <mm.h>
#include <string.h>

struct yksh_controler *yksh_controler;

void yksh_init()
{
        yksh_controler = (struct yksh_controler *)kmalloc(sizeof(struct yksh_controler));
        yksh_controler->cp_x = 0;
        yksh_controler->cp_y = 0;
}


void put_character(u8_t c, u8_t fcolor, u8_t backcolor, u8_t x, u8_t y)
{
        volatile u16_t *where;
        where = (volatile u16_t *)TEXT_VIDEO_RAM_ADDR + (y * 80 + x) ;
        *where = c | (((backcolor << 4) | (fcolor & 0x0f)) << 8);
}


static void clear_current_line()
{
        memset(
                TEXT_VIDEO_RAM_ADDR +
                (((yksh_controler->cp_y * TEXT_SCREEN_WIDTH) + yksh_controler->cp_x) * 2),
                0,
                2 * TEXT_SCREEN_WIDTH
                );
}

static void scroll(u8_t offset)
{
        memcpy(
                TEXT_VIDEO_RAM_ADDR,
                TEXT_VIDEO_RAM_ADDR +
                (TEXT_SCREEN_WIDTH * sizeof(u16_t)), (TEXT_SCREEN_HEIGHT - offset)
                * (TEXT_SCREEN_WIDTH * sizeof(u16_t))
                );
        clear_current_line();
}

static void move_cursor(u16_t p)
{
        io_out8(FRAMEBUF_COMMAND_PORT, FRAMEBUF_HIGH_BYTE);
        io_out8(FRAMEBUF_DATA_PORT, (p >> 8) & 0x00ff);
        io_out8(FRAMEBUF_COMMAND_PORT, FRAMEBUF_LOW_BYTE);
        io_out8(FRAMEBUF_DATA_PORT, p & 0x00ff);
}

static void update_cursor()
{
        move_cursor((yksh_controler->cp_y * SCREEN_WIDTH) + (yksh_controler->cp_x));
}

static void move_right()
{
        yksh_controler->cp_x++;
        update_cursor();
}

static void new_indent()
{
        yksh_controler->cp_x = 0;
        if(yksh_controler->cp_y >= 24)
                scroll(1);
        else
                yksh_controler->cp_y++;
        update_cursor();
}

void put_char(char ch)
{
        if(ch == '\n'){
                new_indent();
                return;
        }
        
        put_character(ch, 0x07, 0x00, yksh_controler->cp_x, yksh_controler->cp_y);
        move_right();
}

static void print_no_indent(const char *str)
{
        while (*str)
		put_char(*str++);
}

void puts(const char *str)
{
        print_no_indent(str);
        new_indent();
}



/*
 *=======================================================================================
 *int2char関数
 *文字コードを文字に変換する
 *=======================================================================================
 */
static void int2char(char *str, int value){
	str[0] = (char)value;
	str[1] = '\0';
}

//数値を16進数文字列に変換する
//flag: 大文字なら1, 小文字なら0
static void int2hex(char *str, int value) {

	int i, n, zero = 0, mask = 0x0f;
	char charctor = 'a';

	for(i = 0; i < 8; i++) {
		n = value >> (7-i)*4;

		if(!zero && n != 0){
			/*
			 *0以外が初めて来た
			 */
			zero = 1;
		}else if(!zero){
			/*
			 *0ではない
			 */
			continue;
		}

		if((n & mask) >= 10) {
			/*
			 *文字に変換
			 */
			*str++ = charctor + (n & mask)-10;
		}else{
			/*
			 *数字に変換
			 */
			*str++ = '0' + (n & mask);
		}
	}

	/*
	 *最後にヌル文字
	 */
	*str = '\0';
}

/*
 *=======================================================================================
 *10進数の指定された桁を返す関数
 *=======================================================================================
 */
static int dec_digit(int value, int n){
	int i;
	for(i = 0; i < n-1; i++){
		value /= 10;
	}
	return value % 10;
}

/*
 *=======================================================================================
 *int2dec関数
 *普通に10進数を文字列にする関数
 *=======================================================================================
 */
static void int2dec(char *str, int value) {
	int i;
	char zero = 1;

	if(!value){
		str[0] = '0';
		str[1] = '\0';
		return;
	}

	if(value < 0){
		str[0] = '-';
		str += 1;
		value = -value;
	}

	for(i = 0;i < 10;i++){
		if(zero && dec_digit(value, 10-i) != 0)
			zero = 0;
		if(!zero)
			*str++ = '0' + dec_digit(value, 10-i);
	}

	*str = '\0';
}

/*
 *=======================================================================================
 *int2str関数
 *普通に数字を文字列にする関数
 *=======================================================================================
 */
static void int2str(char *str, int value) {
	char *p = (char *)value;
	strcpy(str, p);
}

static int _sprintf(char *str, const char *format, int *argv){
	int count, i, argc = 0;
	char buf[50];
	const char *p = format;

	/*
	 *バッファをゼロクリア
	 */
	memset(buf, 0, 40);

	for(count = 0; *p != '\0'; p++) {
		switch(*p) {
		case '%':
			switch(p[1]) {
			case 'd':
				int2dec(buf, argv[argc++]);
				break;
			case 'c':
				int2char(buf, argv[argc++]);
				break;
			case 'x':
				int2hex(buf, argv[argc++]);
				break;
			case 's':
				int2str(buf, argv[argc++]);
				break;
			}

			/*
			 *変換結果をコピー
			 */
			for(i = 0;buf[i] != '\0';i++,count++)
				*str++ = buf[i];
			p++;
			break;
		default:
			*str++ = *p;
			count++;
		}
	}

	str[count] = '\0';

	return count;
}


/*
 *=======================================================================================
 *zero_str関数
 *文字列を\0で埋める関数
 *=======================================================================================
 */
void zero_str(char *str){
	while(*str != '\0')
		*str++ = 0;
}

/*
 *=======================================================================================
 *zeroclear_8array関数
 *配列をゼロクリアする関数
 *=======================================================================================
 */
void zeroclear_8array(i8_t *array, u32_t length){
	u32_t i;
	for(i = 0;i < length;i++)
		array[i] = 0;
}

/*
 *=======================================================================================
 *zeroclear_short_array関数
 *配列をゼロクリアする関数
 *=======================================================================================
 */
void zeroclear_16array(i16_t *array, u32_t length){
	u32_t i;
	for(i = 0;i < length;i++)
		array[i] = 0;
}

/*
 *=======================================================================================
 *zeroclear_short_array関数
 *配列をゼロクリアする関数
 *=======================================================================================
 */
void zeroclear_32array(i32_t *array, u32_t length){
	u32_t i;
	for(i = 0;i < length;i++)
		array[i] = 0;
}

int sprintf(char *str, const char *format, ...)
{
        int *argv = (int *)(&str + 2);
        return _sprintf(str, format, argv);
}

void printk(const char *format, ...)
{
        int *argv = (int *)(&format + 1);
        char *buf = (char *)kmalloc(4096);
        memset(buf, 0, 4096);
        _sprintf(buf, format, argv);
        print_no_indent(buf);
        kfree(buf, 4096);
}
