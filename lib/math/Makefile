OBJS = basis.s basis.o

all: $(OBJS) Makefile
	@echo 'BUILD MATH DIRECTORY'
	rm *.s
	mv *.o ..

%.s: %.c
	@$(CC) -S $*.c $(BINOPT) -o $*.s

%.o: %.s
	@$(AS) $*.s -o $*.o
