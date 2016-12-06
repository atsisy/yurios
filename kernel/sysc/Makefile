BINOPT  = -nostdlib -Wl,--oformat=binary -fno-builtin
DST = ../../dst
OBJS = $(DST)/read.s $(DST)/read.o $(DST)/write.s $(DST)/write.o \
	$(DST)/sleep.s $(DST)/sleep.o

all: $(OBJS) Makefile
	@echo 'BUILD SYSC DIRECTORY'

$(DST)/%.s: %.c
	@gcc -m32 -Wall -S $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	@as --32 $(DST)/$*.s -o $(DST)/$*.o
