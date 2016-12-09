DST = ../../dst
OBJS = $(DST)/print.o $(DST)/strlib.o

all: $(OBJS) Makefile
	@echo 'BUILD YURIC DIRECTORY'

$(DST)/%.o: %.c
	@$(CC) -Ofast $*.c $(BINOPT) -o $(DST)/$*.o
