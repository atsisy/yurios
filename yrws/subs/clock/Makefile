YRWS = ../..
OBJS = $(YRWS)/bar_clock.o

all: $(OBJS) Makefile
	@echo 'BUILD YRWS/LIB DIRECTORY'

$(YRWS)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(YRWS)/$*.o
