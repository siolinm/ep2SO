#!/bin/zsh

make
pkill ep2
time ./ep2 $1 $2 d 2> $3 & tail -f $3
echo "Acabou"
