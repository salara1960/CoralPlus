#!/bin/bash
number=`cat build_number`
let number++
echo "$number" | tee build_number #<-- output and save the number back to file
dat=`date +%d-%m-%Y\ %H:%M:%S`
out="$number $dat"
echo '#define BUILD "'$out'"' | tee build_number.h
