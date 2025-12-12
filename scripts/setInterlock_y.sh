#!/bin/bash

SCRIPT_PATH="/Users/tohoku/PAPILLON_DAQ/papillon/RBCP/RunTool"

<<COMMENTOUT
echo "### Register Interlock ###"
Interlock=$1
if [ ${Interlock} -eq 0 ]; then
	echo "Set Register Interlock: 0 (LOW;danger case)"
elif [ ${Interlock} -eq 1 ]; then
	echo "Set Register Interlock: 1 (HIGH;safe case)"
else
	echo "[!!!ERROR!!!] Invalid argument. Please 0(LOW;danger case) or 1(HIGH;safe case)"
	exit 1
fi
echo -e "wrb 0x70 ${Interlock}\nrd 0x70 1" >& ./CntrlInterlock/callPAPILLONItrlk.txt
COMMENTOUT

LINECounter=0
while read LINE
do
	if [ $((++LINECounter)) -eq 3 ]; then
		echo ">> ${LINE}"
	fi
done < <(${SCRIPT_PATH}/CntrlInterlock/callPAPILLONItrlk_y.zsh)