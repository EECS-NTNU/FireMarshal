CC := gcc
CROSS_COMPILE := riscv64-unknown-elf-

BARE_CFLAGS=-DBAREMETAL -mcmodel=medany -Wall -O1 -g -fno-common -fno-builtin-printf
BARE_LDFLAGS=-static -nostdlib -nostartfiles -lgcc -g

STRIDE_OFFSET=64

CFLAGS=-O2 -g
LDFLAGS=-static -T linux_reloc.ld

.PHONY: all clean

all: bare-bwtest bwtest bare-bwstride bwstride

bwtest bwstride: % : %.o
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) $^ -o $@

bwstride.o: CFLAGS += -DSTRIDED -DSTRIDE_OFFSET=$(STRIDE_OFFSET)
bwtest.o bwstride.o: bwtest.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(DEFINES) -c $^ -o $@

bare-bwtest bare-bwstride: % : %.o firesim_crt.o firesim_syscalls.o
	$(CROSS_COMPILE)$(CC) -T firesim_link.ld $(BARE_LDFLAGS) $^ -o $@

firesim_syscalls.o: %.o : %.c firesim_util.h firesim_encoding.h
	$(CROSS_COMPILE)$(CC) $(BARE_CFLAGS) -c $< -o $@

firesim_crt.o: %.o : %.S
	$(CROSS_COMPILE)$(CC) $(BARE_CFLAGS) -c $< -o $@

bare-bwstride.o: BARE_CFLAGS += -DSTRIDED -DSTRIDE_OFFSET=$(STRIDE_OFFSET)
bare-bwtest.o bare-bwstride.o: bwtest.c
	$(CROSS_COMPILE)$(CC) $(BARE_CFLAGS) $(DEFINES) -c $< -o $@


clean:	
	rm -f *.o
	rm -f bare-bwtest bwtest bare-bwstride bwstride
