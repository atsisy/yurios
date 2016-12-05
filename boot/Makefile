BIN = ipl.bin asmhead.bin
DST = ../dst

all: $(BIN) Makefile
	@mv *.bin $(DST)

%.bin: %.asm
	nasm $*.asm -o $*.bin
