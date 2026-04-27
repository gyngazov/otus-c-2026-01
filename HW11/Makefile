TGT=ascii-art
CFLAGS=-Wall -Wextra -Wpedantic -std=c11

OBJS=main.o

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

${TGT}: ${OBJS}
	gcc -o ${TGT} $^ ${LFLAGS}

clean:
	rm -f ${OBJS} ${TGT} ./a.out
