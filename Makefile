LDFLAGS=-lSDL -lGL -lGLU -lm
CFLAGS=-Wall -g -O0 -DDEBUG

all: metro

metro: metro.o

clean:
	rm -f metro metro.o
