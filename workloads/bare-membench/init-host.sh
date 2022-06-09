#!/bin/bash

make -C bwtest clean
make -C bwtest pointless-bwtest

make -C pointers clean
make -C pointers pointless-pointers
