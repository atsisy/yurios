OBJS = $(DST)/yurilib.o $(DST)/sh_cmd.o $(DST)/end.o
DST = ../../dst

all: $(OBJS) Makefile
	cd file && make all
	@echo 'BUILD YURI DIRECTORY'

$(DST)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(DST)/$*.o
