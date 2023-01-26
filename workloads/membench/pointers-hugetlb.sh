#!/bin/bash

echo "Björn: this benchmark does not use hugepages because the target binary is statically linked and any heap allocation is not going through to hugepagefs. I would propose to push the Linux kernel to at least 5.14 which enables transparent huge page support for RISC-V, though the Linux kernel crashes currently with Chipyard/FireSim from 5.10 onwards which might be caused by a 2 MiB alignment introduced in this version." >&2

# Create 1GB of 2 MB tlb pages
hugeadm --pool-pages-min 2MB:1GB --add-temp-swap

# Enable HugeTLB for heap memory of this process
hugectl --heap /opt/pointers/pointers

poweroff
