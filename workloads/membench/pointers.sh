#!/bin/bash
set +x

firesim-start-trigger
/opt/pointers/pointers
firesim-end-trigger

poweroff
