TGT=que
LFLAGS=-lpq
CFLAGS=-Wall -Wextra -Wpedantic -std=c11 -Iinclude -g

OBJS=q1.o thsque.o copy.o

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

${TGT}: ${OBJS}
	gcc -o ${TGT} $^ ${LFLAGS}

clean:
	rm -f ${OBJS} ${TGT} ./a.out
