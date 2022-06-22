#!/bin/bash

make -C overlay/opt/bwtest/ clean
make -C overlay/opt/bwtest/ bwtest 
make -C overlay/opt/bwtest/ DEFINES=-DSTREAM_SIZE=$((512 * 1024 * 1024)) bwstride

make -C overlay/opt/pointers/ clean
make -C overlay/opt/pointers/ pointers

