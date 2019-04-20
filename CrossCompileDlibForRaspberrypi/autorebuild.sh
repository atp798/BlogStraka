#!/usr/bin/env bash
rm * -r -f
cmake -DCMAKE_TOOLCHAIN_FILE=/home/raspberrypi3b+/pi.cmake ../examples/ 
awk 'BEGIN { cmd="cp -ri ../dlib_build   ./  "; print "n" |cmd; }'
cp ../../../autorebuild.sh ./
