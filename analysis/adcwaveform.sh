#!/bin/bash

#DATANAME="./rowdata/ADCINTdevelop.0812.800mVpp.3MHz.1_to_6ch"
#DATANAME="./rowdata/ADCINTdevelop.0316.test1"
#DATANAME="./rowdata/latency.0316.test1"
#DATANAME="./rowdata/latency.0414_debugmode"
#DATANAME="./rowdata/latency.0421_20230420fw"
#DATANAME="./rowdata/DAQtest.20231201"
#DATANAME="./rowdata/DAQtest.20231202"
#DATANAME="./rowdata/DAQtest.20231202_5"
#DATANAME="./rowdata/DAQtest.v20231202_1207_ch8"
#DATANAME="./rowdata/DAQtest.v20231202_1207_8"
#DATANAME="./rowdata/DAQtest.v20231207_1208_4"
#DATANAME="./rowdata/DAQtest.v20231208_1208_1"
#DATANAME="./rowdata/DAQtest.v20231211_1211_8"
#DATANAME="./rowdata/DAQtest.v20231224_1224_2"
DATANAME="./rowdata/DAQtest.v20231225_1225_7"


./encoder.sh ${DATANAME}
if [ $? -eq 0 ]; then
  root -l 'adcwaveform.C("'${DATANAME}'")'
else
  echo "--> Stopped."
  exit 2
fi