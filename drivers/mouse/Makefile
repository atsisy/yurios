DST = ../../dst
OBJS = $(DST)/mouse.o

all: $(OBJS) Makefile
	@echo 'BUILD DRIVERS/MOUSE DIRECTORY'

$(DST)/%.o: %.c
	$(CC) $*.c $(BINOPT) -o $(DST)/$*.o
