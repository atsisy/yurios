LINKPATH  = ../../../ls
APP = ../../../application
DST = ../../../dst
YURIEXE = main.o

all: $(YURIEXE) Makefile
	@echo 'BUILD YURISCRIPT DIRECTORY'
	@ld -r -melf_i386 main.o $(DST)/yrsysc.o $(DST)/print.o $(DST)/strlib.o -o yrs.o
	@ld -melf_i386 -o $(APP)/yrs.yx --script=$(LINKPATH)/app.ls yrs.o
	@mv *.o $(DST)

%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $*.o

%.o: %.asm
	@nasm -f elf $*.asm -o $*.o
