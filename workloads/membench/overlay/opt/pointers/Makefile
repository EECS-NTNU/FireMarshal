CC=gcc
CROSS_COMPILE=riscv64-unknown-elf-
CFLAGS=-DBAREMETAL -mcmodel=medany -Wall -O1 -g -fno-common -fno-builtin-printf
LDFLAGS=-static -nostdlib -nostartfiles -lgcc -g

SRC = $(filter-out firesim_%,$(wildcard *.c))
OBJ = $(patsubst %.c,%.o,$(SRC))

.PHONY: all clean

all: pointless-pointers pointers

pointers: pointless-pointers.c
	riscv64-unknown-linux-gnu-$(CC) -O2 -g $< -o $@

pointless-pointers : pointless-pointers.o firesim_crt.o firesim_syscalls.o
	$(CROSS_COMPILE)$(CC) -T firesim_link.ld $(LDFLAGS) $^ -o $@

firesim_syscalls.o: %.o : %.c firesim_util.h firesim_encoding.h
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f pointless-pointers pointers
