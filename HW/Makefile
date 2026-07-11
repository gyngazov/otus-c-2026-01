TGT=que
LFLAGS=
CFLAGS=-Wall -Wextra -Wpedantic -std=c11 -Iinclude

OBJS=q.o thsque.o

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

${TGT}: ${OBJS}
	gcc -o ${TGT} $^ ${LFLAGS}

clean:
	rm -f ${OBJS} ${TGT} ./a.out
