DST = ../../dst
OBJS = $(DST)/print.o $(DST)/strlib.o $(DST)/entry.o

all: $(OBJS) Makefile
	@echo 'BUILD YURIC DIRECTORY'

$(DST)/%.o: %.c
	@$(CC) -Ofast $*.c $(BINOPT) -o $(DST)/$*.o
