OBJS = mtask.o init.o tp_switch.o send.o

all: $(OBJS) Makefile
	@echo 'BUILD KERNEL/PROC DIRECTORY'
	@ld -melf_i386 -r $(OBJS) -o $(DST)/proc.o
	rm -f *.o

%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $*.o
