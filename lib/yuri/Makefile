DST = ../../dst
OBJS = $(DST)/yurilib.s $(DST)/yurilib.o $(DST)/sh_cmd.s $(DST)/sh_cmd.o $(DST)/display.s $(DST)/display.o

all: $(OBJS) Makefile
	@echo 'BUILD YURI DIRECTORY'

$(DST)/%.s: %.c
	@gcc -m32 -Wall -S $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	@as --32 $(DST)/$*.s -o $(DST)/$*.o
