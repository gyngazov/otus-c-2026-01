TGT=sql
CFLAGS=-Wall -Wextra -Wpedantic -std=c11
LIBS=-lconfuse -lm -lsqlite3

OBJS=${TGT}.o sqlite.o config.o

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

${TGT}: ${OBJS}
	gcc -o ${TGT} $^ ${LIBS}

clean:
	rm -f ${OBJS} ${TGT} ./a.out