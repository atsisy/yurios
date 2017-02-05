DST = ../../dst
OBJS = $(DST)/print.o $(DST)/strlib.o $(DST)/entry.o $(DST)/yrsysc.o

all: $(OBJS) Makefile
	@echo 'BUILD YURIC DIRECTORY'

$(DST)/%.o: %.c
	@$(CC) -Ofast $*.c $(BINOPT) -o $(DST)/$*.o

$(DST)/%.o: %.asm
	@nasm -f elf $*.asm -o $(DST)/$*.o
