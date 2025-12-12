#!/bin/zsh

cd /home/daq/RBCP/
echo "load ./setup/mux_8.txt\n quit" | python rbcpshell_v2.py 192.168.10.16 4660
sleep 1
echo "load ./setup/mux_8.txt\n quit" | python rbcpshell_v2.py 192.168.11.16 4660
