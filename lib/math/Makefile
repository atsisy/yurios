OBJS = basis.s basis.o

all: $(OBJS) Makefile
	@echo 'BUILD MATH DIRECTORY'
	rm *.s
	mv *.o ..

%.s: %.c
	@gcc -Wall -Os -m32 -S $*.c $(BINOPT) -o $*.s

%.o: %.s
	@as --32 $*.s -o $*.o
