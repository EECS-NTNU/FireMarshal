CC := gcc
CROSS_COMPILE := riscv64-unknown-elf-

BARE_CFLAGS=-DBAREMETAL -mcmodel=medany -Wall -O1 -g -fno-common -fno-builtin-printf
BARE_LDFLAGS=-static -nostdlib -nostartfiles -lgcc -g

STRIDE_OFFSET=64

CFLAGS=-O2 -g
LDFLAGS=-static -T linux_reloc.ld

.PHONY: all clean

all: bare-bwtest bwtest bare-bwstride bwstride bare-bwstride-one-loop bare-bwtest-one-loop

bwtest bwstride: % : %.o
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) $^ -o $@

bwstride.o: CFLAGS += -DSTRIDED -DSTRIDE_OFFSET=$(STRIDE_OFFSET)
bwtest.o bwstride.o: bwtest.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(DEFINES) -c $^ -o $@

bare-bwtest bare-bwstride bare-bwtest-one-loop bare-bwstride-one-loop bare-bwstride-noadd: % : %.o firesim_crt.o firesim_syscalls.o
	$(CROSS_COMPILE)$(CC) -T firesim_link.ld $(BARE_LDFLAGS) $^ -o $@

firesim_syscalls.o: %.o : %.c firesim_util.h firesim_encoding.h
	$(CROSS_COMPILE)$(CC) $(BARE_CFLAGS) -c $< -o $@

firesim_crt.o: %.o : %.S
	$(CROSS_COMPILE)$(CC) $(BARE_CFLAGS) -c $< -o $@

bare-bwstride.o bare-bwstride-noadd.s bare-bwstride-one-loop.o: BARE_CFLAGS += -DSTRIDED -DSTRIDE_OFFSET=$(STRIDE_OFFSET)
bare-bwstride-one-loop.o bare-bwtest-one-loop: BARE_CFLAGS += -DONE_LOOP
bare-bwtest.o bare-bwstride.o bare-bwtest-one-loop.o bare-bwstride-one-loop.o: bwtest.c
	$(CROSS_COMPILE)$(CC) $(BARE_CFLAGS) $(DEFINES) -c $< -o $@
bare-bwstride-noadd.s: bwtest.c
	$(CROSS_COMPILE)$(CC) $(BARE_CFLAGS) -S $(DEFINES) -c $< -o $@

bare-bwstride-noadd.o: bare-bwstride-noadd.s
	$(CROSS_COMPILE)as -c $< -o $@



clean:	
	rm -f *.o
	rm -f bare-bwtest bwtest bare-bwstride bwstride
