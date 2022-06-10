CC := gcc
CROSS_COMPILE := riscv64-unknown-elf-

BARE_CFLAGS=-DBAREMETAL -mcmodel=medany -Wall -O1 -g -fno-common -fno-builtin-printf
BARE_LDFLAGS=-static -nostdlib -nostartfiles -lgcc -g

CFLAGS=-O2 -g -DUSE_HEAP
LDFLAGS=-static -T linux_reloc.ld

.PHONY: all clean

all: bare-pointers pointers


pointers: pointers.o
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) $^ -o $@

pointers.o: %.o : %.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c $^ -o $@

bare-pointers : % : %.o firesim_crt.o firesim_syscalls.o
	$(CROSS_COMPILE)$(CC) -T firesim_link.ld $(BARE_LDFLAGS) $^ -o $@

firesim_syscalls.o: %.o : %.c firesim_util.h firesim_encoding.h
	$(CROSS_COMPILE)$(CC) $(BARE_CFLAGS) -c $< -o $@

firesim_crt.o: %.o : %.S
	$(CROSS_COMPILE)$(CC) $(BARE_CFLAGS) -c $< -o $@

bare-pointers.o: pointers.c
	$(CROSS_COMPILE)$(CC) $(BARE_CFLAGS) -c $< -o $@


clean:
	rm -f *.o
	rm -f bare-pointers pointers
