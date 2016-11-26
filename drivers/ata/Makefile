BINOPT	= -nostdlib -Wl,--oformat=binary -fno-builtin
DST = ../../dst
OBJS = $(DST)/ata_init.s $(DST)/ata_init.o $(DST)/ata_write.s $(DST)/ata_write.o \
$(DST)/ata_read.s $(DST)/ata_read.o $(DST)/ata_back.s $(DST)/ata_back.o $(DST)/prepare_wr.s $(DST)/prepare_wr.o

all: $(OBJS) Makefile
	echo "BUILD ATA DIRECTORY"

$(DST)/%.s: %.c
	gcc -m32 -Wall -S $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	as --32 $(DST)/$*.s -o $(DST)/$*.o
