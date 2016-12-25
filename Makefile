OBJS = dst/kernel_kernel.o dst/keyboard.o	dst/timer.o dst/shell.o \
	dst/yurilib.o dst/history.o dst/string.o dst/sh_cmd.o \
	dst/basis.o dst/ata_init.o dst/ata_write.o \
	dst/ata_read.o dst/ata_back.o dst/prepare_wr.o dst/display.o dst/shfunc.o dst/yrfs.o \
	dst/read.o dst/write.o dst/sleep.o dst/get_clock.o dst/fat_getsize.o \
	dst/set_clock.o dst/mouse.o dst/yrws.o dst/addsmt.o dst/mem_read.o

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
	cd yrws && make all
	ld -melf_i386 -o dst/boot.bin --script=ls/kernel.ls $(OBJS)
	cat dst/asmhead.bin dst/boot.bin > dst/yuli.sys
	mformat -f 1440 -C -B dst/ipl.bin -i yuli.img ::
	mcopy dst/yuli.sys -i yuli.img ::
	mcopy yuri_doc.txt -i yuli.img ::
	mcopy application/cli.yx -i yuli.img ::
	mcopy application/cli_app2.yx -i yuli.img ::
	mcopy application/yrs.yx -i yuli.img ::
	mcopy yuri.yim -i yuli.img ::
	mcopy an.yim -i yuli.img ::
	mcopy LICENSE -i yuli.img ::

clean :
	rm -f ./dst/*.s
	rm -f ./dst/*.o
	rm -f ./dst/yuli.sys
	rm -f ./application/*.yx

time:
	time make all
