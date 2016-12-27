DST = ../dst
OBJS = $(DST)/history.o $(DST)/shell.o  $(DST)/shfunc.o $(DST)/type_prompt.o $(DST)/interp.o

all: $(OBJS) Makefile
	@echo 'BUILD MID DIRECTORY'

$(DST)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(DST)/$*.o
