YRWS = ..
OBJS = $(YRWS)/yrws_main.o $(YRWS)/cursor.o $(YRWS)/boot_subproc.o

all: $(OBJS) Makefile
	@echo 'BUILD YRWS/CORE DIRECTORY'

$(YRWS)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(YRWS)/$*.o
