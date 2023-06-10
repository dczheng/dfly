#!/bin/sh
make &&
kldload ./radix-test.ko
kldunload radix-test.ko
make clean
