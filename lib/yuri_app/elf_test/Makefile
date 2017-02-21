LINKPATH  = ../../../ls
APP = ../../../application
YURIEXE = main.o

all: $(YURIEXE) Makefile
	@echo 'BUILD YURIAPP DIRECTORY'
	@ld -melf_i386 -o $(APP)/elf_test.yx --script=$(LINKPATH)/appyx.lds main.o $(DST)/libyuri.a --nmagic
	@mv *.o $(DST)

%.o: %.c
	@$(CC) $*.c -Wl,--oformat=binary -Wall -o $*.o
