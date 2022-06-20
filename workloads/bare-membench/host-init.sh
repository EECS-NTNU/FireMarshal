#!/bin/bash


make -C bwtest clean
make -C bwtest bare-bwtest bare-bwstride
cp -a bwtest/bare-bwtest ./
cp -a bwtest/bare-bwstride ./

make -C pointers clean
make -C pointers bare-pointers
cp -a pointers/bare-pointers ./

[ ! -e ../../images/bare-membench-pointers.img ] && ln -rs dummy.img ../../images/bare-membench-pointers.img
[ ! -e ../../images/bare-membench-pointers-bin ] && ln -rs bare-pointers ../../images/bare-membench-pointers-bin

[ ! -e ../../images/bare-membench-bwtest.img ] && ln -rs dummy.img ../../images/bare-membench-bwtest.img
[ ! -e ../../images/bare-membench-bwtest-bin ] && ln -rs bare-bwtest ../../images/bare-membench-bwtest-bin

[ ! -e ../../images/bare-membench-bwstride.img ] && ln -rs dummy.img ../../images/bare-membench-bwstride.img
[ ! -e ../../images/bare-membench-bwstride-bin ] && ln -rs bare-bwstride ../../images/bare-membench-bwstride-bin

exit 0
