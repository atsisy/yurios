DST = ../dst
OBJS = $(DST)/start.o $(DST)/graphic.o $(DST)/dsctbl.o \
	$(DST)/int.o  $(DST)/fifo.o $(DST)/memory.o $(DST)/mtask.o \
	$(DST)/timer.o  $(DST)/sys_call.o $(DST)/default_font.o $(DST)/core.o \
	$(DST)/cpuinfo.o $(DST)/t_clock.o

all: $(OBJS) Makefile
	@echo 'BUILD KERNEL DIRECTORY'
	cd fs && make all
	cd sysc && make all

$(DST)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(DST)/$*.o

$(DST)/%.o: %.asm
	@nasm -f elf $*.asm -o $(DST)/$*.o
