#!/bin/bash

ETH_IF=enp9s0f2

sudo brctl addbr br0
sudo ip addr flush dev ${ETH_IF}
sudo brctl addif br0 ${ETH_IF}

sudo tunctl -t tap0 -u $(whoami)
sudo brctl addif br0 tap0

sudo ifconfig ${ETH_IF} up
sudo ifconfig tap0 up
sudo ifconfig br0 up

sudo ifconfig br0 172.25.218.202

sudo iptables -I FORWARD -m physdev --physdev-is-bridged -j ACCEPT
