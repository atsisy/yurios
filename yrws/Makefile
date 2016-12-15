DST = ../dst
OBJS = yrws_main.o

all: $(OBJS) Makefile
	@echo 'BUILD YRWS DIRECTORY'
	@ld -melf_i386 -r yrws_main.o -o $(DST)/yrws.o
	@rm -f *.o

%.o: %.c
	$(CC) $*.c $(BINOPT) -o $*.o
