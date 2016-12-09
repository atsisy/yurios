DST = ../../dst
OBJS = $(DST)/ata_init.o $(DST)/ata_write.o \
 $(DST)/ata_read.o $(DST)/ata_back.o $(DST)/prepare_wr.o

all: $(OBJS) Makefile
	@echo 'BUILD ATA DIRECTORY'

$(DST)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(DST)/$*.o
