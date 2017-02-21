BIN = ipl.bin asmhead.bin

all: $(BIN) Makefile
	@mv *.bin $(DST)

%.bin: %.asm
	nasm $*.asm -o $*.bin
