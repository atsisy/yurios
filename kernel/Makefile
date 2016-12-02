BINOPT	= -nostdlib -Wl,--oformat=binary -fno-builtin
DST = ../dst
OBJS = $(DST)/start.s $(DST)/start.o $(DST)/graphic.s $(DST)/graphic.o \
	$(DST)/dsctbl.s $(DST)/dsctbl.o \
	$(DST)/int.s $(DST)/int.o $(DST)/fifo.s $(DST)/fifo.o \
	$(DST)/memory.s $(DST)/memory.o $(DST)/mtask.s $(DST)/mtask.o \
	$(DST)/timer.s $(DST)/timer.o $(DST)/sys_call.s $(DST)/sys_call.o \
	$(DST)/default_font.s $(DST)/default_font.o $(DST)/core.o

all: $(OBJS) Makefile
	cd fs && make all
	echo "BUILD KERNEL DIRECTORY"

$(DST)/%.s: %.c
	gcc -m32 -Wall -S $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	as --32 $(DST)/$*.s -o $(DST)/$*.o

$(DST)/%.o: %.asm
	nasm -f elf $*.asm -o $(DST)/$*.o
