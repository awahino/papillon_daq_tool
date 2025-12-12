#!/bin/bash

#############################
# How to use:
# sedi@MacBook-Pro:~$ ./converter.sh 0xEBF4
#
#############################

### 16 bit ###
readonly MINUS1=65535

### Input hex results (should be add 0x)
### e.g. 0xEBF4
INPUT=$@
echo $INPUT
if [ -z "$INPUT" ]; then
  echo "Input is empty. Please specify the input with HEX."
  echo "### e.g. ### "
  echo " sedi@MacBook-Pro:~$ ./converter.sh 0xEBF4"
  exit 1
fi

### Get decimal results 
### e.g. 0xEBF4 = 0d60404
RESULT=(`echo $(($INPUT))`)
echo $RESULT

### Get convrted result
### e.g. 0xEBF4 = 0d60404 = 65535 - 5131
###                       = -1 - 5131    (65535 = -1 for 16 bit)
###                       = -5132
CONVERTEDRESULT=$((RESULT-$MINUS1))
#echo $CONVERTEDRESULT

CONVERTEDRESULT=$((CONVERTEDRESULT-1))
#echo $CONVERTEDRESULT

### Express the result with the unit mm
CONVERTEDRESULT=(`awk "BEGIN { print $CONVERTEDRESULT/100 }"`)
echo "Result = $CONVERTEDRESULT mm"