YRWS = ..
OBJS = $(YRWS)/yrws_main.o $(YRWS)/cursor.o

all: $(OBJS) Makefile
	@echo 'BUILD YRWS/CORE DIRECTORY'

$(YRWS)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(YRWS)/$*.o
