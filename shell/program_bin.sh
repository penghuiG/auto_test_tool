#!/bin/bash

#--------------------------------------------------
#	program binfile, usage: ./program firmware.bin/hex
#	chip:s32k3系列
#	start addr:0x00400000
#--------------------------------------------------

cmdfile="./tmp.jlink"
start_addr=400000

if [ $# -lt 1 ]; then
	echo "Usage: $0 firmware.bin/hex"
	exit 1
fi

if [ ! -f $1 ]; then
	echo "file '$1' not exit, Please select correct file"		
	exit 1
fi

filename=$(basename "$1")
extension="${filename##*.}"  

if [ ! -f $cmdfile ]; then
	touch $cmdfile

	echo h > $cmdfile
	if [ $extension = "bin" ]; then	
		echo loadfile $1 $start_addr >> $cmdfile
	else
		echo loadfile $1 >> $cmdfile
	fi
	echo r >> $cmdfile
	echo g >> $cmdfile
	echo qc >> $cmdfile
	
fi

JLinkExe -device S32K314 -autoconnect 1 -if SWD -speed 4000 -CommanderScript $cmdfile

if [ -f $cmdfile ]; then
	rm $cmdfile
fi

