#! /bin/bash

#ENCORDER() {
  
#  prefix=$1

#  od -h $prefix.bin >& $prefix.txt
#}

#ENCORDER ./rowdata/adctest.sin.100k.1event
#ENCORDER ./rowdata/adctest.sin.100k.2event
#ENCORDER ./rowdata/adctest.sin.100k.10event
#ENCORDER ./rowdata/adctest.sin.1M.500mVpp.20event
#ENCORDER ./rowdata/adctest.sin.1M.100mVpp.10event
#ENCORDER ./rowdata/adctest.sin.100k.100mVpp.10event
#ENCORDER ./rowdata/adctest.sin.100k.100mVpp.10event.1ch
#ENCORDER ./rowdata/adctest.sin.100.100mVpp.10event.ch0
#ENCORDER ./rowdata/adctest.sin.100k.100mVpp.10event.ch0
#ENCORDER ./rowdata/adctest.sin.100k.100mVpp.10event.ch0.fwv2
#ENCORDER ./rowdata/adctest.sin.200k.500mVpp.10event.ch0.fwv2
#ENCORDER ./rowdata/debug.sin.500k.500mVpp.10event.ch0
#ENCORDER ./rowdata/adctest.sin.500k.500mVpp.10event.ch0.fwv4
#ENCORDER ./rowdata/adctest.sin.500k.500mVpp.10event.ch0.vmetag.fwv4


#ENCORDER ./rowdata/adctest.0514
#ENCORDER ./rowdata/adctest.0613
#ENCORDER ./rowdata/ADCINTdevelop.0617
#ENCORDER ./rowdata/ADCINTdevelop.0726
#ENCORDER ./rowdata/ADCINTdevelop.0727
#ENCORDER ./rowdata/ADCINTdevelop.0729

#ENCORDER ./rowdata/ADCINTdevelop.0812.800mVpp.3MHz.1_to_6ch
#ENCORDER ./rowdata/ADCINTdevelop.0812.800mVpp.3MHz.9_to_14ch
#ENCORDER ./rowdata/ADCINTdevelop.0812.800mVpp.3MHz.17_to_22ch
#ENCORDER ./rowdata/ADCINTdevelop.0812.800mVpp.3MHz.9_to_14_23ch
#ENCORDER ./rowdata/ADCINTdevelop.0812.800mVpp.3MHz.12_to_13ch

#ENCORDER ./rowdata/ADCINTdevelop.0812.200mV.3MHz.9_to_14ch

#ENCORDER ./rowdata/ADCINTdevelop.0313.test


prefix=$1
echo ${prefix}
od -h ${prefix}.bin >& ${prefix}.txt
if [ $? -eq 0 ]; then
  echo "Data file is converted."
else
  echo "Data file conversion is aborted."
  exit 2
fi
