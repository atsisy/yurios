LINKPATH  = ../../ls
BINOPT	  = -nostdlib -Wl,--oformat=binary -fno-builtin -ffreestanding
YURIEXE = asm_cli.o cli.s cli.o cli_app2.s \
	cli_app2.o
APP = ../../application
DST = ../../dst

all: $(YURIEXE) Makefile
	@echo 'BUILD YURI_APP DIRECTORY'
	@ld -melf_i386 -o $(APP)/cli.yx --script=$(LINKPATH)/app.ls asm_cli.o $(DST)/print.o cli.o
	@ld -melf_i386 -o $(APP)/cli_app2.yx --script=$(LINKPATH)/app.ls asm_cli.o $(DST)/print.o $(DST)/file.o cli_app2.o
	rm *.s
	mv *.o ..

%.s: %.c
	@gcc -m32 -Wall -S $*.c $(BINOPT) -o $*.s

%.o: %.s
	@as --32 $*.s -o $*.o

%.o: %.asm
	@nasm -f elf $*.asm -o $*.o
