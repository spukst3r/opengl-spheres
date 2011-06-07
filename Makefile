CC=gcc
ARGS=$(shell pkg-config gtk+-2.0 glib-2.0 gtkgl-2.0 --cflags --libs)
LIBS=-lGL

all: lab

lab: lab.o
	${CC} ${ARGS} ${LIBS} $< -o $@

lab.o: lab.c
	${CC} ${ARGS} ${LIBS} -c $<

clean:
	rm *.o lab

