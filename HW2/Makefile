TARGET=wttr
CFLAGS=-Wall -Wextra -Wpedantic -std=c11
LFLAGS=-lcurl
OBJS=cJSON.o parser.o weather.o main.o

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

${TARGET}: ${OBJS}
	gcc -o ${TARGET} $^ ${LFLAGS}

clean:
	rm -f ${OBJS} a.out
