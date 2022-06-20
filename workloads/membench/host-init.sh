#!/bin/bash

make -C overlay/opt/bwtest/ clean
make -C overlay/opt/bwtest/ bwtest bwstride

make -C overlay/opt/pointers/ clean
make -C overlay/opt/pointers/ pointers

