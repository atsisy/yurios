DRIVERS = ..
OBJS = $(DRIVERS)/mouse.o

all: $(OBJS) Makefile
	@echo 'BUILD DRIVERS/MOUSE DIRECTORY'

$(DRIVERS)/%.o: %.c
	$(CC) $*.c $(BINOPT) -o $(DRIVERS)/$*.o
