YRWS = ..
OBJS = $(YRWS)/layer.o $(YRWS)/layer_map.o $(YRWS)/layer_redraw.o

all: $(OBJS) Makefile
	@echo 'BUILD YRWS/LAYER DIRECTORY'

$(YRWS)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(YRWS)/$*.o
