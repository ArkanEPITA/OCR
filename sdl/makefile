# Simple SDL mini Makefile

CC=gcc

CPPFLAGS= `pkg-config --cflags sdl` -MMD
CFLAGS= -Wall -Wextra -std=c99 -O3
LDFLAGS=
LDLIBS= `pkg-config --libs sdl` -lSDL_image -lm

all: filtres find_letter

filtres: pixel_operations.o filtres.o

filtres.o: pixel_operations.h

find_letter: pixel_operations.o find_letter.o

find_letter.o: pixel_operations.h

clean:
	${RM} *.o
	${RM} *.d
	${RM} filtres
	${RM} find_letter

# END
