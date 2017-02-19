DST = ../../dst
OBJS = fs.o open.o wdata.o inode.o seek.o stat.o close.o find.o flist.o byte2sector.o fs_write.o fremove.o dir.o \
rdir.o chdir.o

all: $(OBJS) Makefile
	@echo 'BUILD FS DIRECTORY'
	@ld -melf_i386 -r $(OBJS) -o $(DST)/yrfs.o
	@rm -f *.o

%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $*.o
