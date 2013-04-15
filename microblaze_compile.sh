#!/bin/bash
#. /home/charizard/.bashrc

if [ $# -lt 1 ]
then
  echo "What do you think you're doing? I expect:"
  echo " "
  echo " 1nd argument: file to compile using the mbgcc."
  echo " [Optional] 3rd argument: output filename"
  echo " "
  echo "..what's wrong with you?"
  echo "N.B.: No 2nd argument? Output filename will be input filename with appended .exe"
  exit -1
fi

source_file=$1

if [ $# -eq 1 ]
then
compiled_file="$1.exe"
else
compiled_file=$2
fi

echo $compiled_file


microblaze-unknown-linux-gnu-g++ -mcpu=v8.10.a -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -ffixed-r31 --sysroot /usr/local/lib/mbgcc/microblaze-unknown-linux-gnu/sys-root $source_file -o $compiled_file

