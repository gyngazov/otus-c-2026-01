TGT=chardev
CFLAGS=-Wall -Wextra -Wpedantic -std=c11
INC=\
-I/usr/src/linux-headers-6.8.0-60-generic/arch/x86/include/generated \
-I/usr/src/linux-hwe-6.8-headers-6.8.0-60/include \
-I/usr/src/linux-hwe-6.8-headers-6.8.0-60/arch/x86/include

OBJS=symdev.o

%.o : %.c
	gcc -c $(CFLAGS) $(INC) $< -o $@

${TGT}: ${OBJS}
	gcc -o ${TGT} $^

clean:
	rm -f ${OBJS} ${TGT} ./a.out