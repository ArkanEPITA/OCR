# Simple SDL mini Makefile

CC=gcc

CPPFLAGS= `pkg-config --cflags sdl` -MMD
CFLAGS= -Wall -Wextra -Werror -std=c99 -O3
LDFLAGS=
LDLIBS= `pkg-config --libs sdl` -lSDL_image

all: grayscale black_and_white

grayscale: pixel_operations.o grayscale.o
black_and_white: pixel_operations.o black_and_white.o

grayscale.o: pixel_operations.h
black_and_white.o: black_and_white.o

clean:
	${RM} *.o
	${RM} *.d
	${RM} display
	${RM} grayscale
	${RM} black_and_white

# END
