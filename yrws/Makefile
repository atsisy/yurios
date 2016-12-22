DST = ../dst
OBJS = yrws_main.o cursor.o layer.o layer_redraw.o draw_window.o layer_map.o \
	puttext.o

all: $(OBJS) Makefile
	@echo 'BUILD YRWS DIRECTORY'
	@ld -melf_i386 -r $(OBJS) -o $(DST)/yrws.o
	@rm -f *.o

%.o: %.c
	gcc -O2 -m32 -c -Wa,--32 $*.c $(BINOPT) -o $*.o
