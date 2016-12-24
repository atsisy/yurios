DST = ../../dst
LINKPATH  = ../../ls
YURIEXE = $(DST)/yrsysc.o cli.o cli_app2.o
APP = ../../application

all: $(YURIEXE) Makefile
	@echo 'BUILD YURI_APP DIRECTORY'
	ar rcs libyuri.a $(DST)/yrsysc.o $(DST)/print.o
	ld -melf_i386 -o $(APP)/cli.yx --script=$(LINKPATH)/app.ls cli.o libyuri.a --nmagic
	ld -melf_i386 -o $(APP)/cli_app2.yx --script=$(LINKPATH)/app.ls cli_app2.o libyuri.a --nmagic
	cd yrs && make all
	mv *.o ..
	rm *.a

%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $*.o

$(DST)/%.o: %.asm
	@nasm -f elf $*.asm -o $(DST)/$*.o

