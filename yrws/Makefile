DST = ../dst
OBJS = yrws_main.o cursor.o layer.o layer_redraw.o draw_window.o layer_map.o \
	puttext.o create_window.o yim.o boot_subproc.o bar_clock.o

all: Makefile
	cd core && make all
	cd layer && make all
	cd lib && make all
	cd window && make all
	cd subs && make all
	@echo 'BUILD YRWS DIRECTORY'
	@ld -melf_i386 -r $(OBJS) -o $(DST)/yrws.o
	@rm -f *.o
