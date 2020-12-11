#Makefile

CC=gcc -rdynamic

CPPFLAGS =  `pkg-config gtk+-3.0 --cflags sdl` -MMD
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS =
LDLIBS = `pkg-config gtk+-3.0 --libs sdl` -lSDL_image -lm

SRC = neural_network.c
OBJ= $(SRC:.c=.o)
DEP= $(SRC:.c=.d)


all: neural_network

main: $(OBJ)

clean:
		$(RM) $(OBJ) $(OBJ_TESTS) $(DEP) $(DEP_TESTS) neural_network && clear
