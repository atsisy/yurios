OBJS =  $(DST)/read.o $(DST)/write.o $(DST)/sleep.o $(DST)/fork.o
DST = ../../dst

all: $(OBJS) Makefile
	@echo 'BUILD SYSC DIRECTORY'

$(DST)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(DST)/$*.o
