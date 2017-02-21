OBJS = $(DST)/get_clock.o $(DST)/set_clock.o $(DST)/timer.o

all: $(OBJS) Makefile
	@echo 'BUILD KERNEL/TIME DIRECTORY'

$(DST)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(DST)/$*.o
