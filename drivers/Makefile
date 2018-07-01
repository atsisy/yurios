OBJS = keyboard.o ata_init.o ata_write.o ata_read.o ata_back.o prepare_wr.o

all: keyboard.o Makefile
	@echo 'BUILD DRIVERS DIRECTORY'
	cd ata && make all
	#cd mouse && make all
	ld -melf_i386 -r $(OBJS) -o $(DST)/drivers.o
	rm *.o

%.o: %.c
	$(CC) $*.c $(BINOPT) -o $*.o
