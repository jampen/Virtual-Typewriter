CC=c++
COPTS=-lncurses


main.o: main.cc
	${CC} main.cc ${COPTS}

./a.out: main.o
	${CC} -c main.o
