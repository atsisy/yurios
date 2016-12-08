DST = ../../dst
OBJS = $(DST)/print.o $(DST)/strlib.s $(DST)/strlib.o

all: $(OBJS) Makefile
	@echo 'BUILD YURIC DIRECTORY'

$(DST)/%.s: %.c
	@$(CC) -S -Ofast $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	@$(AS) $(DST)/$*.s -o $(DST)/$*.o
