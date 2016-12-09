DST = ../../dst
OBJS = $(DST)/fs.o $(DST)/open.o $(DST)/wdata.o $(DST)/inode.o \
 $(DST)/seek.o $(DST)/stat.o $(DST)/close.o $(DST)/find.o \
 $(DST)/flist.o

all: $(OBJS) Makefile
	@echo 'BUILD FS DIRECTORY'

$(DST)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(DST)/$*.o
