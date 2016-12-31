OBJS = dst/kernel_kernel.o dst/drivers.o	dst/timer.o dst/yrsh.o \
	dst/yurilib.o dst/string.o dst/sh_cmd.o dst/proc.o \
	dst/basis.o dst/fork.o dst/display.o dst/yrfs.o dst/ope_new.o \
	dst/read.o dst/write.o dst/sleep.o dst/get_clock.o dst/fat_getsize.o \
	dst/set_clock.o dst/yrws.o dst/addsmt.o dst/mem_read.o

BINOPT = -nostdlib -Wl,--oformat=binary -fno-builtin -ffreestanding -Wall -nostdinc
CC = gcc -O0 -m32 -c -Wa,--32
export BINOPT
export CC

all:
	make clean
	cd drivers && make all
	cd boot && make all
	cd kernel && make all
	cd yrsh && make all
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
	mcopy LICENSE -i yuli.img ::
	mcopy ex.sh -i yuli.img ::

clean :
	rm -f ./dst/*.s
	rm -f ./dst/*.o
	rm -f ./dst/yuli.sys
	rm -f ./application/*.yx

time:
	time make all
