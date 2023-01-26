#!/bin/bash
set +x

firesim-start-trigger
/opt/bwtest/bwtest
firesim-end-trigger

poweroff
