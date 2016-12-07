DST = ../../dst
OBJS = $(DST)/string.s $(DST)/string.o

all: $(OBJS) Makefile
	@echo 'BUILD STRING DIRECTORY'

$(DST)/%.s: %.c
	@$(CC) -S $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	@$(AS) $(DST)/$*.s -o $(DST)/$*.o
