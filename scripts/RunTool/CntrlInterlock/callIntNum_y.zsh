#!/bin/zsh

ip="192.168.11.16"

echo "load ./RunTool/CntrlInterlock/callIntNum.txt\n quit" | python rbcpshell_v2.py ${ip} 4660