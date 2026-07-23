TGT=que
LFLAGS=-lpq -L/usr/lib64/mysql -lmysqlclient -lz -lzstd -lssl -lcrypto -lresolv -lm -lpthread -lrt
CFLAGS=-Wall -Wextra -Wpedantic -std=gnu11 -Iinclude -I/usr/include/mysql -m64 -g

OBJS=loader.o thsque.o copy.o ranger.o

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

${TGT}: ${OBJS}
	gcc -o ${TGT} $^ ${LFLAGS}

clean:
	rm -f ${OBJS} ${TGT} ./a.out
