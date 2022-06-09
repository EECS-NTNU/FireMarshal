#!/bin/sh

export CROSS_COMPILE=riscv64-unknown-linux-gnu-

#firesim tracev triggers
make -C trigger

# pmapelf
make -C pmapelf

# pperf
make -C pperf clean pperf
