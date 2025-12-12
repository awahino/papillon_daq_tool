#!/bin/bash

SCRIPT_PATH="/Users/tohoku/PAPILLON_DAQ/papillon/RBCP/RunTool"

LINECounter=0
while read LINE
do
	if [ $((++LINECounter)) -lt 7 ]; then
		echo ">> ${LINE}"
	fi
done < <(${SCRIPT_PATH}/CntrlInterlock/resetInterlock_x.zsh)