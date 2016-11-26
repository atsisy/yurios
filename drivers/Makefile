BINOPT = -nostdlib -Wl,--oformat=binary -fno-builtin
DST = ../dst
OBJS = $(DST)/keyboard.s $(DST)/keyboard.o

all: $(OBJS) Makefile
	cd ata && make all
	echo "BUILD DRIVER DIRECTORY"

$(DST)/%.s: %.c
	gcc -m32 -Wall -S $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	as --32 $(DST)/$*.s -o $(DST)/$*.o
