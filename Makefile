#Chinese Checkers Makefile

OBJ = main.o text_ui.o data_struct.o gen.o
CC = gcc
LIBS = -lncurses -lm
CFLAGS = -Wall
OUT = ltl.out

all: ${OBJ}
	${CC} ${CFLAGS} ${LIBS} -o ${OUT} ${OBJ}

data_struct.o:  data_struct.h
gen.o:          data_struct.h
text_ui.o:	text_ui.h data_struct.h gen.h
main.o:		text_ui.h data_struct.h gen.h

.PHONY: clean
clean:
	-rm ${OUT} ${OBJ}

