#!/bin/bash

ETH_IF=enp9s0f2

sudo iptables -D FORWARD -m physdev --physdev-is-bridged -j ACCEPT
sudo brctl delif br0 tap0
sudo tunctl -d tap0

sudo brctl delif br0 ${ETH_IF}
sudo ifconfig br0 down
sudo brctl delbr br0

sudo ifconfig ${ETH_IF} up

if [[ $1 == "-d" ]]; then
  sudo dhclient -v ${ETH_IF}
fi
