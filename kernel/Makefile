DST = ../dst
OBJS = start.o graphic.o dsctbl.o int.o fifo.o memory.o \
	sys_call.o default_font.o core.o cpuinfo.o timer_int.o register.o stream.o

all: $(OBJS) Makefile
	@echo 'BUILD KERNEL DIRECTORY'
	@ld -melf_i386 -r $(OBJS) -o $(DST)/kernel_kernel.o
	cd fs && make all
	cd sysc && make all
	cd time && make all
	cd proc && make all
	cd cxx && make all
	rm -f *.o

%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $*.o

%.o: %.asm
	@nasm -f elf $*.asm -o $*.o
