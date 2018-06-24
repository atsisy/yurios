OBJS = basis.o
DST = ../../dst

all: $(OBJS) Makefile
	@echo 'BUILD MATH DIRECTORY'
	mv *.o ..

%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $*.o
