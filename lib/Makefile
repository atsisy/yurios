BINOPT  = -nostdlib -Wl,--oformat=binary -fno-builtin
DST = ../dst

all: $(OBJS) Makefile
	echo "BUILD LIB DIRECTORY"
	cd math && make all
	cd yuri && make all
	cd yuri_app && make all
	cd string && make all
	mv *.o $(DST)

$(DST)/%.s: %.c
	gcc -m32 -Wall -S $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	as --32 $(DST)/$*.s -o $(DST)/$*.o
