#!/bin/zsh

ip="192.168.10.16"

echo "load ./RunTool/CntrlInterlock/callPAPILLONItrlk.txt\n quit" | python rbcpshell_v2.py ${ip} 4660