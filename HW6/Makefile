TARGET=gtk
CFLAGS=-Wall -Wextra -Wpedantic -std=c11
LFLAGS=$(shell pkg-config --libs gtk4) 
PKG=$(shell pkg-config --cflags gtk4)
OBJS=main.o

%.o : %.c
	gcc -c $(CFLAGS) $(PKG) $< -o $@

${TARGET}: ${OBJS}
	gcc -o ${TARGET} $^ ${LFLAGS}

clean:
	rm -f ${OBJS} ${TARGET}
