CC=gcc
ARGS=$(shell pkg-config gtk+-2.0 glib-2.0 gtkgl-2.0 --cflags --libs)
LIBS=-lGL

OBJECTS=lab.o gui.o gl.o

all: lab

lab: ${OBJECTS}
	${CC} ${ARGS} ${LIBS} ${OBJECTS} -o $@

%.o: %.c
	${CC} ${ARGS} ${LIBS} -c $<

clean:
	rm *.o lab

