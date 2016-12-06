BINOPT  = -nostdlib -Wl,--oformat=binary -fno-builtin -ffreestanding
DST = ../../dst
OBJS = $(DST)/print.o $(DST)/strlib.s $(DST)/strlib.o $(DST)/file.o

all: $(OBJS) Makefile
	@echo 'BUILD YURIC DIRECTORY'

$(DST)/%.s: %.c
	@gcc -m32 -Wall -S $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	@as --32 $(DST)/$*.s -o $(DST)/$*.o
