CC = gcc
CFLAGS= -Wall -Wextra -std=c99
LDFLAGS=
LDLIBS= -lm

all: xor
	
xor: ${OBJ}

-include ${DEP}

clean:
	rm -f ${OBJ} ${DEP}
	rm -f xor
	rm -f *.txt
