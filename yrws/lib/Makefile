YRWS = ..
OBJS = $(YRWS)/puttext.o $(YRWS)/yim.o $(YRWS)/draw.o

all: $(OBJS) Makefile
	@echo 'BUILD YRWS/LIB DIRECTORY'

$(YRWS)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(YRWS)/$*.o
