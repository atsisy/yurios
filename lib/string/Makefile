DST = ../../dst
OBJS = $(DST)/string.s $(DST)/string.o

all: $(OBJS) Makefile
	@echo 'BUILD STRING DIRECTORY'

$(DST)/%.s: %.c
	@$(CC) -m32 -Wall -S $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	@as --32 $(DST)/$*.s -o $(DST)/$*.o
