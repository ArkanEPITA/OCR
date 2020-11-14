CC = gcc
CFLAGS=
LDFLAGS=
LDLIBS= -lm

all: xor
	
xor: ${OBJ}

-include ${DEP}

clean:
	rm -f ${OBJ} ${DEP}
	rm -f xor
	
