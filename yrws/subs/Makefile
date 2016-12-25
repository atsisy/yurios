YRWS = ..

all: Makefile
	cd clock && make all
	@echo 'BUILD YRWS/LIB DIRECTORY'

$(YRWS)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(YRWS)/$*.o
