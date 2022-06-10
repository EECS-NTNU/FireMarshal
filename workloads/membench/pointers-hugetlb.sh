#!/bin/bash

# Create 1GB of 2 MB tlb pages
hugeadm --pool-pages-min 2MB:1GB --add-temp-swap

# Enable HugeTLB for heap memory of this process
hugectl --heap /opt/pointers/pointers

poweroff
