#!/bin/zsh

ip="192.168.10.17"

sleep 1
echo 'load ./old/setup.20171219/setup1.txt\n quit' | python rbcpshell_v2.py ${ip} 4660
sleep 1
echo "load ./old/setup.20171219/setup2.txt\n quit" | python rbcpshell_v2.py ${ip} 4660
sleep 1
echo "load ./old/setup.20171219/setup3.txt\n quit" | python rbcpshell_v2.py ${ip} 4660
sleep 1
echo "load ./old/setup.20171219/setup4.txt\n quit" | python rbcpshell_v2.py ${ip} 4660
sleep 1
echo "load ./old/setup.20171219/setup5.txt\n quit" | python rbcpshell_v2.py ${ip} 4660
sleep 1

##
echo "load ./old/setup.20171219/strip_pos_06_y.txt\n quit" | python rbcpshell_v2.py ${ip} 4660
sleep 1

##
echo "load ./old/setup.20171219/mux_8.txt\n quit" | python rbcpshell_v2.py ${ip} 4660
sleep 1
