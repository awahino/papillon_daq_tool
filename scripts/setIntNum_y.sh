#!/bin/bash

SCRIPT_PATH="/Users/tohoku/PAPILLON_DAQ/papillon/RBCP/RunTool"


IntNum=$1
if [ ${IntNum} -eq 0 ]; then
	echo "0x23 is not changed."
else
	echo -e "wrb 0x23 ${IntNum}\nrd 0x23 1" >& ./RunTool/CntrlInterlock/callIntNum.txt	
fi


LINECounter=0
while read LINE
do
	if [ $((++LINECounter)) -eq 3 ]; then
		echo ">> ${LINE}"
	fi
done < <(${SCRIPT_PATH}/CntrlInterlock/callIntNum_y.zsh)