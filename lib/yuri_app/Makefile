DST = ../../dst
LINKPATH  = ../../ls
YURIEXE = $(DST)/yrsysc.o cli.o cli_app2.o
APP = ../../application

all: $(YURIEXE) Makefile
	@echo 'BUILD YURI_APP DIRECTORY'
	ld -melf_i386 -o $(APP)/cli.yx --script=$(LINKPATH)/app.ls $(DST)/yrsysc.o $(DST)/print.o cli.o
	#ld -melf_i386 -o $(APP)/cli_app2.yx --script=$(LINKPATH)/app.ls $(DST)/yrsysc.o $(DST)/print.o cli_app2.o
	cd yrs && make all
	mv *.o ..

%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $*.o

$(DST)/%.o: %.asm
	@nasm -f elf $*.asm -o $(DST)/$*.o
