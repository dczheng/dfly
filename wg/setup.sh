#!/bin/sh

LISTEN_IP="192.168.2.81"
LISTEN_PORT="9010"
WG0_IP="192.168.10.10/24"
WG1_IP="192.168.10.20/24"
ALLOWED_IPS="0.0.0.0/0"

ROOT=`dirname $0`

case $1 in 
    dfly-module-reload)
        cd /usr/src/sys/net/wg
        kldunload if_wg
        make clean
        make && make install \
        && kldload if_wg
    ;;

    gen-conf)
        KEY0=`wg genkey`
        KEY1=`wg genkey`
        PUB_KEY0=`echo ${KEY0} | wg pubkey`
        PUB_KEY1=`echo ${KEY1} | wg pubkey`
        cat > wg0.conf << EOF
[Interface]
PrivateKey = ${KEY0}
ListenPort = ${LISTEN_PORT}
Address = ${WG0_IP}

[Peer]
PublicKey = ${PUB_KEY1}
AllowedIPS = ${ALLOWED_IPS}
EOF
        cat > wg1.conf << EOF
[Interface]
PrivateKey = ${KEY1}
Address = ${WG1_IP}

[Peer]
PublicKey = ${PUB_KEY0}
AllowedIPS = ${ALLOWED_IPS}
EndPoint = ${LISTEN_IP}:${LISTEN_PORT}
EOF
    ;;

    up) wg-quick up ${ROOT}/$2.conf ;;
    down) wg-quick down ${ROOT}/$2.conf ;;

    *)
        echo "Error" $1
    ;;
esac
