YRSH = ..
OBJS = $(YRSH)/yrs_load.o $(YRSH)/yrs_src_init.o

all: $(OBJS) Makefile
	@echo 'BUILD YRSH/SCRIPT DIRECTORY'

$(YRSH)/%.o: %.c
	@$(CC) $*.c $(BINOPT) -o $(YRSH)/$*.o
