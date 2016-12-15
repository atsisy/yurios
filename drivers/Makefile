DST = ../dst
OBJS = $(DST)/keyboard.o

all: $(OBJS) Makefile
	@echo 'BUILD DRIVERS DIRECTORY'
	cd ata && make all
	cd mouse && make all

$(DST)/%.o: %.c
	$(CC) $*.c $(BINOPT) -o $(DST)/$*.o
