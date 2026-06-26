TGT=cube
CFLAGS=-Wall -Wextra -Wpedantic -std=c11
LIBS=-lglut -lm -lGL

OBJS=${TGT}.o

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

${TGT}: ${OBJS}
	gcc -o ${TGT} $^ ${LIBS}

clean:
	rm -f ${OBJS} ${TGT} ./a.out