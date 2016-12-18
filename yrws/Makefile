DST = ../dst
OBJS = yrws_main.o cursor.o layer.o layer_redraw.o draw_window.o

all: $(OBJS) Makefile
	@echo 'BUILD YRWS DIRECTORY'
	@ld -melf_i386 -r $(OBJS) -o $(DST)/yrws.o
	@rm -f *.o

%.o: %.c
	$(CC) $*.c $(BINOPT) -o $*.o
