DST = ../../dst
OBJS = $(DST)/yurilib.o $(DST)/sh_cmd.o $(DST)/display.o $(DST)/end.o

all: $(OBJS) Makefile
	cd file && make all
	@echo 'BUILD YURI DIRECTORY'

$(DST)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(DST)/$*.o
