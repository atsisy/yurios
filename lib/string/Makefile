OBJS = $(DST)/string.o $(DST)/string_buffer.o
DST = ../../dst

all: $(OBJS) Makefile
	@echo 'BUILD STRING DIRECTORY'

$(DST)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(DST)/$*.o
