#!/bin/bash

SCRIPT_PATH="/Users/tohoku/PAPILLON_DAQ/papillon/RBCP/RunTool"

LINECounter=0
while read LINE
do
	if [ $((++LINECounter)) -eq 2 ]; then
		echo ">> ${LINE}"
	fi
done < <(${SCRIPT_PATH}/CntrlInterlock/readInterlock_x.zsh)