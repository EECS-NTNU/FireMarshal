#!/bin/sh

export CROSS_COMPILE=riscv64-unknown-linux-gnu-

#firesim tracev triggers
make -C trigger

# pmapelf
make -C pmapelf

# pperf-debug
make -C pperf DEFINES="-DDEBUG" clean pperf
mv pperf/pperf pperf/pperf-debug

# pperf
make -C pperf clean pperf
