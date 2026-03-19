TARGET=logger
CFLAGS=-Wall -Wextra -Wpedantic -std=c11
LFLAGS=-lpthread
OBJS=main.o

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

${TARGET}: ${OBJS}
	gcc -o ${TARGET} $^ ${LFLAGS} -L. logger.a

clean:
	rm -f ${OBJS} ${TARGET} ./a.out
