BINOPT	= -nostdlib -Wl,--oformat=binary -fno-builtin
DST = ../dst
OBJS = $(DST)/history.s $(DST)/history.o $(DST)/shell.s $(DST)/shell.o

all: $(OBJS) Makefile
	echo 'BUILD MID DIRECTORY'

$(DST)/%.s: %.c
	gcc -m32 -S -Wall $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	as --32 $(DST)/$*.s -o $(DST)/$*.o
