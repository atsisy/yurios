DST = ../../dst
LINKPATH  = ../../ls
YURIEXE = $(DST)/yrsysc.o cli.o cli_app2.o
APP = ../../application
CC = gcc -O0 -m32 -c -Wa,--32,-a,-ad

all: $(YURIEXE) Makefile
	@echo 'BUILD YURI_APP DIRECTORY'
	ar rcs libyuri.a $(DST)/yrsysc.o $(DST)/print.o $(DST)/strlib.o
	ld -melf_i386 -o $(APP)/cli.yx --script=$(LINKPATH)/app.ls cli.o libyuri.a --nmagic -Map $(DST)/cli.map
	ld -melf_i386 -o $(APP)/cli_app2.yx --script=$(LINKPATH)/app.ls cli_app2.o libyuri.a --nmagic -Map $(DST)/cli_app2.map
	cd yrs && make all
	mv *.o ..
	rm *.a

%.o: %.c
	$(CC) $*.c $(BINOPT) -o $*.o > $(DST)/$*.lst

$(DST)/%.o: %.asm
	@nasm -f elf $*.asm -o $(DST)/$*.o

