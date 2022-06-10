#!/bin/bash

make -C bwtest clean
make -C bwtest bare-bwtest

make -C pointers clean
make -C pointers bare-pointers
