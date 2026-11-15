#ltl Makefile

OBJ = main.o text_ui.o data_struct.o gen.o
CC = gcc
LIBS = -lncurses -ltinfo
CFLAGS = -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600
OUT = ltl.out

all: ${OBJ}
	${CC} ${CFLAGS} -o ${OUT} ${OBJ} ${LIBS}

data_struct.o:  data_struct.h
gen.o:          text_ui.h data_struct.h gen.h
text_ui.o:	text_ui.h data_struct.h
main.o:		text_ui.h data_struct.h gen.h

.PHONY: clean
clean:
	-rm ${OUT} ${OBJ}

