TGT=parser
LFLAGS=-lglib-2.0
CFLAGS=-Wall -Wextra -Wpedantic -std=c11 -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include

OBJS=main.o hash.o parse.o distributor.o

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

${TGT}: ${OBJS}
	gcc -o ${TGT} $^ ${LFLAGS}

clean:
	rm -f ${OBJS} ${TGT} ./a.out
