#!/bin/zsh

ip="192.168.10.16"

sleep 1
echo 'load ./setup/setup_adc1.txt\n quit' | python rbcpshell_v2.py ${ip} 4660
sleep 1
echo "load ./setup/setup_adc2.txt\n quit" | python rbcpshell_v2.py ${ip} 4660
sleep 1
echo "load ./setup/setup_adc3.txt\n quit" | python rbcpshell_v2.py ${ip} 4660
sleep 1
echo "load ./setup/setup_adcclk.txt\n quit" | python rbcpshell_v2.py ${ip} 4660
sleep 1
echo "load ./setup/setup_register.txt\n quit" | python rbcpshell_v2.py ${ip} 4660
sleep 1
echo "load ./setup/pedestal_SSEM06_P101.txt\n quit" | python rbcpshell_v2.py ${ip} 4660
sleep 1

##
echo "load ./setup/strip_pos_06_x.txt\n quit" | python rbcpshell_v2.py ${ip} 4660
sleep 1

##
echo "load ./setup/mux_8.txt\n quit" | python rbcpshell_v2.py ${ip} 4660
sleep 1
