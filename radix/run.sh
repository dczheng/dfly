#!/bin/sh

make &&
kldload ./test_radix.ko &&
kldunload test_radix.ko &&
dmesg

make clean 2>&1 > /dev/null
