LINKPATH  = ../../../ls
APP = ../../../application
DST = ../../../dst
YURIEXE = main.o

all: $(YURIEXE) Makefile
	@echo 'BUILD YURISCRIPT DIRECTORY'
	@ld -melf_i386 -o $(APP)/elf_test.yx --script=$(LINKPATH)/appyx.lds main.o $(DST)/libyuri.a -nmagic
	@mv *.o $(DST)

%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $*.o
