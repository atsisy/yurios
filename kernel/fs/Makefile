DST = ../../dst
OBJS = $(DST)/fs.s $(DST)/fs.o $(DST)/open.s $(DST)/open.o \
 $(DST)/wdata.s $(DST)/wdata.o $(DST)/inode.s $(DST)/inode.o \
 $(DST)/seek.s $(DST)/seek.o $(DST)/stat.s $(DST)/stat.o \
 $(DST)/close.s $(DST)/close.o $(DST)/find.s $(DST)/find.o \
 $(DST)/flist.o

all: $(OBJS) Makefile
	@echo 'BUILD FS DIRECTORY'

$(DST)/%.s: %.c
	@$(CC) -S $*.c $(BINOPT) -o $(DST)/$*.s

$(DST)/%.o: $(DST)/%.s
	@$(AS) $(DST)/$*.s -o $(DST)/$*.o
