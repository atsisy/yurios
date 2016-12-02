BINOPT = -nostdlib -Wl,--oformat=binary -fno-builtin
DST = ../dst
OBJS = $(DST)/keyboard.s $(DST)/keyboard.o

all: $(OBJS) Makefile
	@echo "BUILD DRIVERS DIRECTORY"
	cd ata && make all

$(DST)/%.s: %.c
	gcc -m32 -Wall -S $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	as --32 $(DST)/$*.s -o $(DST)/$*.o
