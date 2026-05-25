TGT=module
CFLAGS=-w#-Wall -Wextra -Wpedantic -std=c11
INC=\
-I/usr/src/kernels/6.19.10-300.fc44.x86_64/arch/x86/include/ \
-I/usr/src/kernels/6.19.10-300.fc44.x86_64/arch/x86/include/generated \
-I/usr/src/kernels/6.19.10-300.fc44.x86_64/include

OBJS=${TGT}.o

%.o : %.c
	gcc -c $(CFLAGS) $(INC) $< -o $@

${TGT}: ${OBJS}
	gcc -o ${TGT} $^

clean:
	rm -f ${OBJS} ${TGT} ./a.out