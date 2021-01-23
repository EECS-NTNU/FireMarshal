#!/bin/sh

if [ -d overlay/usr/local/bin ]; then
	echo "Please delete $(readlink -f overlay/usr/local) to force rebuild of overlay components!"
	exit 0
fi

mkdir -pv overlay/usr/local/bin
export CROSS_COMPILE=riscv64-unknown-linux-gnu-

# perf
make -C "${FIREMARSHAL_LINUX_SRC}/tools/perf" ARCH=riscv64 prefix="$(readlink -f 'overlay/usr/local')" install
