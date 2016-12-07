DST = ../../dst
OBJS = $(DST)/read.s $(DST)/read.o $(DST)/write.s $(DST)/write.o \
	$(DST)/sleep.s $(DST)/sleep.o

all: $(OBJS) Makefile
	@echo 'BUILD SYSC DIRECTORY'

$(DST)/%.s: %.c
	@$(CC) -S $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	@$(AS) $(DST)/$*.s -o $(DST)/$*.o
