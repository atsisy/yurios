YRWS = ..
OBJS = $(YRWS)/puttext.o

all: $(OBJS) Makefile
	@echo 'BUILD YRWS/LIB DIRECTORY'

$(YRWS)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(YRWS)/$*.o
