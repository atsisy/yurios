DRIVERS = ..
OBJS = $(DRIVERS)/ata_init.o $(DRIVERS)/ata_write.o \
 $(DRIVERS)/ata_read.o $(DRIVERS)/ata_back.o $(DRIVERS)/prepare_wr.o

all: $(OBJS) Makefile
	@echo 'BUILD ATA DIRECTORY'

$(DRIVERS)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(DRIVERS)/$*.o
