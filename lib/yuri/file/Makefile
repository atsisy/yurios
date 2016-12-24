DST = ../../../dst
OBJS = $(DST)/addsmt.o $(DST)/mem2hd.o $(DST)/mem_read.o $(DST)/fat_getsize.o

all: $(OBJS) Makefile
	@echo 'BUILD YURI/FILE DIRECTORY'

$(DST)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(DST)/$*.o
