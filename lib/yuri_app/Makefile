DST = ../../dst
LINKPATH  = ../../ls
YURIEXE = $(DST)/yrsysc.o cli.s cli.o cli_app2.s \
	cli_app2.o
APP = ../../application

all: $(YURIEXE) Makefile
	@echo 'BUILD YURI_APP DIRECTORY'
	@ld -melf_i386 -o $(APP)/cli.yx --script=$(LINKPATH)/app.ls $(DST)/yrsysc.o $(DST)/print.o cli.o
	@ld -melf_i386 -o $(APP)/cli_app2.yx --script=$(LINKPATH)/app.ls $(DST)/yrsysc.o $(DST)/print.o cli_app2.o
	cd yrs && make all
	rm *.s
	mv *.o ..

%.s: %.c
	@$(CC) -m32 -Wall -S $*.c $(BINOPT) -o $*.s

%.o: %.s
	@as --32 $*.s -o $*.o

$(DST)/%.o: %.asm
	@nasm -f elf $*.asm -o $(DST)/$*.o
