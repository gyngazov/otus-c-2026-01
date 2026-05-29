TGT=http
CFLAGS=-Wall -Wextra -Wpedantic -std=c11

OBJS=main.o async.o

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

${TGT}: ${OBJS}
	gcc -o ${TGT} $^

clean:
	rm -f ${OBJS} ${TGT} ./a.out