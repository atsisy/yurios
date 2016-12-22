YRWS = ..
OBJS = $(YRWS)/draw_window.o

all: $(OBJS) Makefile
	@echo 'BUILD YRWS/WINDOW DIRECTORY'

$(YRWS)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(YRWS)/$*.o
