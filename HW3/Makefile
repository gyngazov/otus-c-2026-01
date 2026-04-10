TARGET=c-asm
CFLAGS=-Wall -Wextra -Wpedantic -std=c11
OBJS=main.o

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

${TARGET}: ${OBJS}
	gcc -o ${TARGET} $^ ${LFLAGS}

clean:
	rm -f ${OBJS} ${TARGET}
