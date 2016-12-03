BINOPT  = -nostdlib -Wl,--oformat=binary -fno-builtin
DST = ../../dst
OBJS = $(DST)/print.s $(DST)/print.o

all: $(OBJS) Makefile
	@echo 'BUILD YURIC DIRECTORY'

$(DST)/%.s: %.c
	@gcc -m32 -Wall -S $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	@as --32 $(DST)/$*.s -o $(DST)/$*.o
