LINKPATH  = ../../../ls
DST = ../../../dst
APP = ../../../application
YURIEXE = main.o

all: $(YURIEXE) Makefile
	@echo 'BUILD PARROT DIRECTORY'
	@ld -melf_i386 -o $(APP)/parrot.yx --script=$(LINKPATH)/appyx.lds main.o $(DST)/entry.o $(DST)/libyuri.a --nmagic
	@mv *.o $(DST)

%.o: %.c
	@$(CC) $*.c -Wl,--oformat=binary -Wall -o $*.o
