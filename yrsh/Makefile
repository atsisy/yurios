DST = ../dst
OBJS = history.o shell.o shfunc.o type_prompt.o interp.o

all: $(OBJS) Makefile
	@echo 'BUILD MID DIRECTORY'
	@ld -melf_i386 -r $(OBJS) -o $(DST)/yrsh.o
	@rm -f *.o
	
%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $*.o
