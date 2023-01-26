#!/bin/bash
set +x

firesim-start-trigger
/opt/bwtest/bwstride
firesim-end-trigger

poweroff
