DST = ../../dst
OBJS = $(DST)/yurilib.s $(DST)/yurilib.o $(DST)/sh_cmd.s $(DST)/sh_cmd.o $(DST)/display.s $(DST)/display.o

all: $(OBJS) Makefile
	@echo 'BUILD YURI DIRECTORY'

$(DST)/%.s: %.c
	@$(CC) -S $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	@$(AS) $(DST)/$*.s -o $(DST)/$*.o
