# add link -lm when u need to include <math.h> or <cmath>
CC = /usr/bin/g++
LIBS = -lm
OBJS =  Main.o Topology.o FatTree.o Service.o User.o UserSet.o Simulation.o
SOURCE = Main.cpp Topology.cpp FatTree.cpp Service.cpp User.cpp UserSet.cpp Simulation.cpp
MAIN = a.out
CFLAGS = -O3 -Wall -std=c++11 -fopenmp

main:
	${CC} ${CFLAGS} -c ${SOURCE}
	${CC} ${CFLAGS} -o ${MAIN} ${OBJS} ${LIBS}
	cp ${MAIN} ../

debug:
	${CC} -g -o ${MAIN} ${SOURCE}

clean:
	rm -f ${MAIN} ${OBJS} ${ODR_MAIN} ${ODR_OBJS}

