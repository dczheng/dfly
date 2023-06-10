#!/bin/bash

ISO="dfly.iso"
QCOW2="dfly.qcow2"

IFACE="enp49s0"
BR="br0"
TAP="tap0"

OPTS="
    -machine type=q35,accel=kvm
    -smp cpus=4
    -m 4G
    -display curses
    -vga qxl

    -drive file=$QCOW2,if=none,id=disk0,format=qcow2
    -device virtio-blk-pci,drive=disk0

    -netdev tap,ifname=$TAP,id=net0,script=no,downscript=no,vhost=on
    -device virtio-net-pci,netdev=net0,mac=1a:2b:3c:4d:5f:02

    -object rng-random,id=rng0,filename=/dev/urandom
    -device virtio-rng-pci,rng=rng0
"

case $1 in 
    install)
        qemu-img create -f qcow2 $QCOW2 100G
        qemu-system-x86_64 $OPTS \
            -cdrom $ISO \
            -boot order=d
    ;;

    start)
        qemu-system-x86_64 $OPTS
    ;; 

    net-setup)
        echo 1 > /proc/sys/net/ipv4/ip_forward

        systemctl stop networking
        ip link set dev $IFACE up 

        ip link add name $BR type bridge
        ip link set dev $BR up 
        ip link set dev $BR address "1a:2b:3c:4d:5f:01"

        ip tuntap add $TAP mode tap
        ip link set dev $TAP up
        ip link set dev $TAP address "1a:2b:3c:4d:5f:0f"

        ip link set dev $IFACE master $BR
        ip link set dev $TAP master $BR

        dhcpcd $BR
        ip route flush dev $IFACE
    ;;

    net-clean)
        ip link set $IFACE nomaster
        ip tuntap delete $TAP mode tap
        ip link delete $BR type bridge
        systemctl start networking
    ;;

    *)
    echo "Error" $1
    ;;
esac
