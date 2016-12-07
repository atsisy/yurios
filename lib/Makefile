DST = ../dst

all: Makefile
	@echo 'BUILD LIB DIRECTORY'
	cd yuric && make all
	cd math && make all
	cd yuri && make all
	cd string && make all
	cd yuri_app && make all
	mv *.o $(DST)
