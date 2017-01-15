DST = ../dst
OBJS = history.o shell.o shfunc.o type_prompt.o interp.o exec_file.o

all: $(OBJS) Makefile
	@echo 'BUILD YRSH DIRECTORY'
	cd script && make all
	@ld -melf_i386 -r $(OBJS) yrs_load.o yrs_src_init.o -o $(DST)/yrsh.o
	@rm -f *.o

%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $*.o
