TGT=game
CFLAGS=-Wall -Wextra -Wpedantic -std=c11
LIBS=-lSDL2 -lSDL2_image
OBJS=${TGT}.o

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

${TGT}: ${OBJS}
	gcc ${LIBS} -o ${TGT} $^

clean:
	rm -f ${OBJS} ${TGT} ./a.out