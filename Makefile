OBJS = dst/start.o dst/core.o dst/default_font.o dst/graphic.o	\
	dst/dsctbl.o dst/int.o dst/fifo.o dst/keyboard.o \
	dst/memory.o dst/timer.o dst/mtask.o dst/shell.o \
	dst/yurilib.o dst/history.o dst/string.o  dst/sh_cmd.o \
	dst/basis.o dst/sys_call.o dst/ata_init.o dst/ata_write.o \
	dst/ata_read.o dst/ata_back.o dst/prepare_wr.o dst/display.o \
	dst/open.o dst/fs.o dst/inode.o dst/wdata.o dst/shfunc.o \
	dst/cpuinfo.o dst/seek.o dst/stat.o dst/close.o dst/find.o \
	dst/read.o dst/write.o dst/sleep.o dst/flist.o dst/get_clock.o \
	dst/set_clock.o

BINOPT = -nostdlib -Wl,--oformat=binary -fno-builtin -ffreestanding -Wall -nostdinc
CC = gcc -O0 -m32 -c -Wa,--32
export BINOPT
export CC

all:
	make clean
	cd drivers && make all
	cd boot && make all
	cd kernel && make all
	cd mid && make all
	cd lib && make all
	ld -melf_i386 -o dst/boot.bin --script=ls/kernel.ls $(OBJS)
	cat dst/asmhead.bin dst/boot.bin > dst/yuli.sys
	mformat -f 1440 -C -B dst/ipl.bin -i yuli.img ::
	mcopy dst/yuli.sys -i yuli.img ::
	mcopy yuri_doc.txt -i yuli.img ::
	mcopy application/cli.yx -i yuli.img ::
	mcopy application/cli_app2.yx -i yuli.img ::
	mcopy application/yrs.yx -i yuli.img ::

clean :
	rm -f ./dst/*.s
	rm -f ./dst/*.o
	rm -f ./dst/yuli.sys
	rm -f ./application/*.yx

time:
	time make all
