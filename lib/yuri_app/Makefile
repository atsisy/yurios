DST = ../../dst
LINKPATH  = ../../ls
YURIEXE = cli.o cli_app2.o rename.o
APP = ../../application
CC = gcc -O0 -m32 -c -Wa,--32,-a,-ad

all: $(YURIEXE) Makefile
	@echo 'BUILD YURI_APP DIRECTORY'
	@echo 'BUILD libyuri.a'
	ar rcs libyuri.a $(DST)/yrsysc.o $(DST)/print.o $(DST)/strlib.o $(DST)/entry.o
	ld -melf_i386 -o $(APP)/cli.yx --script=$(LINKPATH)/app.lds cli.o libyuri.a --nmagic -Map $(DST)/cli.map
	ld -melf_i386 -o $(APP)/cli_app2.yx --script=$(LINKPATH)/app.lds cli_app2.o libyuri.a --nmagic -Map $(DST)/cli_app2.map
	ld -melf_i386 -o $(APP)/rename.yx --script=$(LINKPATH)/app.lds rename.o libyuri.a --nmagic -Map $(DST)/rename.map
	mv *.a $(DST)
	cd yrs && make all
	cd elf_test && make all
	cd parrot && make all
	mv *.o ..

%.o: %.c
	$(CC) $*.c $(BINOPT) -o $*.o > $(DST)/$*.lst


