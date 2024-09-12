SRC := $(shell ls -1 src/*.c)
OBJS := $(SRC:c=o)
CFLAGS := -Iincludes/

all: $(OBJS)
	gcc -o lvm $(OBJS)
	gcc -O0 -fno-pie -no-pie testprog/testprog.c -o tprog -nostdlib
%.o: %.c
	gcc ${CFLAGS} -o $@ -c $<
clean:
	rm -rf ${OBJS} lvm

.PHONY: all testprog clean
