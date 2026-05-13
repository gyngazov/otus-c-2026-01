TGT=gtk
#LFLAGS=-lrt
PKG=$(shell pkg-config --cflags gtk4)
CFLAGS=-Wall -Wextra -Wpedantic -std=c11

OBJS=main.o

%.o : %.c
	gcc -c $(CFLAGS) $(PKG) $< -o $@

${TGT}: ${OBJS}
	gcc -o ${TGT} $^ ${LFLAGS}

clean:
	rm -f ${OBJS} ${TGT} ./a.out