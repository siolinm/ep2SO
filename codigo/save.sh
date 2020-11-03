#!/bin/bash

make
pkill ep2
# { time ./ep2 $1 $2 $3 2> $4 & tail -f $4 }
{ time ./ep2 $1 $2 $3 2> teste$1-$2-$3.txt
} 2>> relatorio$1-$2-$3.txt
