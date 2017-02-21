OBJS = $(DST)/ope_new.o

all: $(OBJS) Makefile
	@echo 'BUILD CXX DIRECTORY'

$(DST)/%.o: %.cxx
	@g++ -O0 -m32 -c -Wa,--32 $*.cxx $(BINOPT) -o $(DST)/$*.o
