LINKPATH  = ../../../ls
APP = ../../../application
DST = ../../../dst
YURIEXE = main.s main.o

all: $(YURIEXE) Makefile
	@echo 'BUILD YURISCRIPT DIRECTORY'
	@ld -melf_i386 -o $(APP)/yrs.yx --script=$(LINKPATH)/app.ls $(DST)/yrsysc.o $(DST)/print.o main.o

%.s: %.c
	@gcc -m32 -Wall -S $*.c $(BINOPT) -o $*.s

%.o: %.s
	@as --32 $*.s -o $*.o

%.o: %.asm
	@nasm -f elf $*.asm -o $*.o
