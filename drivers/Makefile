DST = ../dst
OBJS = $(DST)/keyboard.s $(DST)/keyboard.o

all: $(OBJS) Makefile
	@echo 'BUILD DRIVERS DIRECTORY'
	cd ata && make all

$(DST)/%.s: %.c
	$(CC) -S $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	$(AS) $(DST)/$*.s -o $(DST)/$*.o
