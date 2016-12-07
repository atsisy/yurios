BINOPT	= -nostdlib -Wl,--oformat=binary -fno-builtin
DST = ../dst
OBJS = $(DST)/history.s $(DST)/history.o $(DST)/shell.s $(DST)/shell.o \
	$(DST)/shfunc.s $(DST)/shfunc.o

all: $(OBJS) Makefile
	@echo 'BUILD MID DIRECTORY'

$(DST)/%.s: %.c
	@$(CC) -S $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	@$(AS) $(DST)/$*.s -o $(DST)/$*.o
