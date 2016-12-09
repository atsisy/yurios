DST = ../../dst
OBJS = $(DST)/string.o

all: $(OBJS) Makefile
	@echo 'BUILD STRING DIRECTORY'

$(DST)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(DST)/$*.o
